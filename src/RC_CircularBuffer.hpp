/**
 * Date: 24.05.2016
 * Author: Toni Uhlig <matzeton@googlemail.com>
 */

#ifndef RC_CIRCULARBUFFER_H
#define RC_CIRCULARBUFFER_H 1

#include <vector>


namespace rc {
template <class T>
class CircularBuffer {

  private:
    T* cBuffer; // Array der Länge "maxElements"
    unsigned int nextIndex; // Feldindex, welcher beim nächsten addElement(...) überschrieben wird
    unsigned int maxElements; // maximale Größe des Puffers

  public:
    CircularBuffer(unsigned int maxElements) {
      nextIndex = 0;
      this->maxElements = maxElements;
      cBuffer = new T[maxElements]; // erzeuge Feld der Größe n (maxElements)
    }

    ~CircularBuffer(void) {
      delete[] cBuffer;
    }

    void addElement(T elem) {
      /* überschreibe das Element an Position "nextIndex" */
      cBuffer[nextIndex] = elem;
      /* nextIndex inkrementieren und dabei auf die Grenze "maxElements" beachten */
      /* % = Modulo */
      auto tmp = nextIndex;
      tmp = ++nextIndex % maxElements;
      nextIndex = tmp;
    }

    unsigned int getNextIndex(void) {
      return nextIndex;
    }

    T getCurrentElement(void) {
      return cBuffer[nextIndex];
    }

};
}

#endif
