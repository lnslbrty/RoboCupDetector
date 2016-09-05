#include <opencv2/opencv.hpp>

#include <time.h>

#include "RC_CircularBuffer.hpp"

template class rc::CircularBuffer<cv::Mat>;
template class rc::cbElement<cv::Mat>;


template <class T>
rc::CircularBuffer<T>::CircularBuffer(unsigned int maxElements) {
  nextIndex         = 0;
  availableElements = 0;
  skippedElements   = 0;
  this->maxElements = maxElements;
  cBuffer = new struct cbElement<T>[maxElements]; // erzeuge Feld der Größe n (maxElements)
  
  time_t tm = time(NULL);
  maxAge = tm;
  for (auto i = maxElements; i > 0; --i) {
    cBuffer[i % maxElements].processed = false;
    cBuffer[i % maxElements].tstamp = tm;
  }
}

template <class T>
rc::CircularBuffer<T>::~CircularBuffer(void) {
  delete[] cBuffer;
}

template <class T>
bool rc::CircularBuffer<T>::addElement(T& elem) {
  bool ret = true;
  std::unique_lock<std::mutex> lck(mtx);
  if (!cBuffer[nextIndex].processed) {
    time_t oldest = cBuffer[nextIndex].tstamp;
    auto newIndex = nextIndex;
    for (auto i = nextIndex+1; i < nextIndex+maxElements; ++i) {
      auto index = i % maxElements;
      if (!cBuffer[index].processed && oldest < cBuffer[index].tstamp) {
        oldest   = cBuffer[index].tstamp;
        newIndex = index;
      }
    }
    if (newIndex != nextIndex)
      skippedElements++;
    nextIndex = newIndex;
    ret = false;
  }

  /* überschreibe das Element an Position "nextIndex" */
  cBuffer[nextIndex].element   = elem;
  /* den entsprechende Index in `alreadyProcessedElements` auf `false` setzen */
  cBuffer[nextIndex].processed = false;
  /* Element einen Zeitstempel verpassen */
  cBuffer[nextIndex].tstamp = time(NULL);
  /* nextIndex inkrementieren und dabei auf die Grenze "maxElements" beachten */
  /* % = Modulo */
  auto tmp = nextIndex;
  tmp = ++nextIndex % maxElements;
  nextIndex = tmp;
  if (availableElements < maxElements)
    availableElements++;
  return ret;
}

template <class T>
bool rc::CircularBuffer<T>::getElement(T& arg) {
  std::unique_lock<std::mutex> lck(mtx);
  if (availableElements == 0) return false; // noch keine Elemente im Puffer
  for (auto i = nextIndex; i < nextIndex+maxElements; ++i) {
    auto index = i % maxElements;
    /* Wurde das Element noch nicht bearbeitet und ist noch nicht zu alt? */
    if (cBuffer[index].processed == false && maxAge <= cBuffer[index].tstamp) {
      cBuffer[index].processed = true;
      arg = cBuffer[index].element;
      availableElements--;
      maxAge = cBuffer[index].tstamp;
      return true;
    }
  }
  return false; // alle Elemente im Puffer wurden bereits mit `getElement()` zurückgegeben
}

template <class T>
std::string rc::CircularBuffer<T>::getInfo(void) {
  std::stringstream out;
  out << "CirularBufferPos: "<<this->getNextIndex()
      << " availableElements: "<<this->getElementCount()
      << " skippedElements: "<<this->getSkipped()
      << " processed: ";
  auto i = nextIndex;
  for (i = 0; i < maxElements; ++i) {
    if (cBuffer[i].processed) {
      out << "*";
    } else out << "-";
  }
  return out.str();
}
