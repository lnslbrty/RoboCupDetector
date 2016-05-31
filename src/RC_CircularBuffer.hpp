/**
 * Date: 24.05.2016
 * Author: Toni Uhlig <matzeton@googlemail.com>
 */

/* ACHTUNG: Diese Klasse ist _NICHT_ `Threadsafe`, d.h. in Multithreaded Umgebungen muss der Entwickler entsprechende Synchronisationsmaßnahmen ergreifen (siehe RC_BlobDetectorFactor) */

#ifndef RC_CIRCULARBUFFER_H
#define RC_CIRCULARBUFFER_H 1

#include <iostream>
#include <vector>


namespace rc {
template <class T>
class CircularBuffer {

  private:
    T * cBuffer; // Array der Länge "maxElements"
    unsigned int nextIndex; // Feldindex, welcher beim nächsten addElement(...) überschrieben wird
    unsigned int maxElements; // maximale Größe des Puffers
    bool * alreadyProcessedElements; // Bool- Feld, welches bereits durch `getElement` zurückgegebe Elemente mit `true` markiert

  public:
    CircularBuffer(unsigned int maxElements) {
      nextIndex = 0;
      this->maxElements = maxElements;
      cBuffer = new T[maxElements]; // erzeuge Feld der Größe n (maxElements)
      alreadyProcessedElements = new bool[maxElements];
      for (auto i = maxElements; i > 0; --i) {
        alreadyProcessedElements[i % maxElements] = false;
      }
    }

    ~CircularBuffer(void) {
      delete[] cBuffer;
      delete[] alreadyProcessedElements;
    }

    void addElement(T& elem) {
      /* überschreibe das Element an Position "nextIndex" */
      cBuffer[nextIndex] = elem;
      /* den entsprechende Index in `alreadyProcessedElements` auf `false` setzen */
      alreadyProcessedElements[nextIndex] = false;
      /* nextIndex inkrementieren und dabei auf die Grenze "maxElements" beachten */
      /* % = Modulo */
      auto tmp = nextIndex;
      tmp = ++nextIndex % maxElements;
      nextIndex = tmp;
    }

    T getCurrentElement(void) {
      long long int index = (nextIndex-1) % maxElements;
      return cBuffer[index];
    }

    unsigned int getNextIndex(void) {
      return nextIndex;
    }

    bool getElement(T& arg) {
      for (auto i = nextIndex; i < nextIndex+maxElements; ++i) {
        auto index = i % maxElements;
        if (alreadyProcessedElements[index] == false) {
          alreadyProcessedElements[index] = true;
          arg = cBuffer[index];
          return true;
        }
      }
      return false; // alle Elemente im Puffer wurden bereits mit `getElement()` zurückgegeben
    }

};
}

#endif
