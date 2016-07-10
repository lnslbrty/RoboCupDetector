/**
 * Date: 24.05.2016
 * Author: Toni Uhlig <matzeton@googlemail.com>
 */


#ifndef RC_CIRCULARBUFFER_H
#define RC_CIRCULARBUFFER_H 1

#include <iostream>
#include <vector>
#include <mutex>

#include <time.h>


namespace rc {
template <class T>
struct cbElement {
  T element;
  bool processed;
  unsigned int id; // TODO: id nutzen, um die totale Ordnung der Bilder nach dem processing beizubehalten
};

template <class T>
class CircularBuffer {

  private:
    std::mutex mtx;
    struct cbElement<T> * cBuffer; // Array der Länge "maxElements"
    unsigned int nextIndex; // Feldindex, welcher beim nächsten addElement(...) überschrieben wird
    unsigned int maxElements; // maximale Größe des Puffers
    unsigned int availableElements; // Anzahl "frischer" Einträge

  public:
    CircularBuffer(unsigned int maxElements);
    ~CircularBuffer(void);

    void addElement(T& elem);

    unsigned int getMaxElementCount(void) { return maxElements; }
    unsigned int getNextIndex(void) { 
      std::unique_lock<std::mutex> lck(mtx);
      return nextIndex;
    }
    unsigned int getElementCount(void) {
      std::unique_lock<std::mutex> lck(mtx);
      return availableElements;
    }

    bool getElement(T& arg);

};
}

#endif
