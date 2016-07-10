#include <opencv2/opencv.hpp>

#include "RC_CircularBuffer.hpp"

template class rc::CircularBuffer<cv::Mat>;
template class rc::cbElement<cv::Mat>;


template <class T>
rc::CircularBuffer<T>::CircularBuffer(unsigned int maxElements) {
  nextIndex = 0;
  availableElements = 0;
  this->maxElements = maxElements;
  cBuffer = new struct cbElement<T>[maxElements]; // erzeuge Feld der Größe n (maxElements)
  
  for (auto i = maxElements; i > 0; --i) {
    cBuffer[i % maxElements].processed = false;
  }
}

template <class T>
rc::CircularBuffer<T>::~CircularBuffer(void) {
  delete[] cBuffer;
}

template <class T>
void rc::CircularBuffer<T>::addElement(T& elem) {
  std::unique_lock<std::mutex> lck(mtx);
  /* überschreibe das Element an Position "nextIndex" */
  cBuffer[nextIndex].element   = elem;
  /* den entsprechende Index in `alreadyProcessedElements` auf `false` setzen */
  cBuffer[nextIndex].processed = false;
  /* nextIndex inkrementieren und dabei auf die Grenze "maxElements" beachten */
  /* % = Modulo */
  auto tmp = nextIndex;
  tmp = ++nextIndex % maxElements;
  nextIndex = tmp;
  if (availableElements < maxElements)
    availableElements++;
}

template <class T>
bool rc::CircularBuffer<T>::getElement(T& arg) {
  std::unique_lock<std::mutex> lck(mtx);
  if (availableElements == 0) return false; // noch keine Elemente im Puffer
  for (auto i = nextIndex; i < nextIndex+maxElements; ++i) {
    auto index = i % maxElements;
    if (cBuffer[index].processed == false) {
      cBuffer[index].processed = true;
      arg = cBuffer[index].element;
      availableElements--;
      return true;
    }
  }
  return false; // alle Elemente im Puffer wurden bereits mit `getElement()` zurückgegeben
}

