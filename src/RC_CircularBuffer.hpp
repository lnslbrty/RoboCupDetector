/**
 * @file    RC_CircularBuffer.hpp
 * @author  Toni Uhlig <matzeton@googlemail.com>
 * @date    24.05.2016
 * @version 1.0
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
  T element;      /** aktuelles Element */
  bool processed; /** bereits zurückgegebenes Element */
};

template <class T>
class CircularBuffer {

  private:
    std::mutex mtx;                 /** Mutex für Vector */
    struct cbElement<T> * cBuffer;  /** Array der Länge `maxElements` */
    unsigned int nextIndex;         /** Feldindex, welcher beim nächsten `addElement(...)` überschrieben wird */
    unsigned int maxElements;       /** maximale Größe des Puffers */
    unsigned int availableElements; /** Anzahl "frischer" Einträge */

  public:
    /**
     * @name Konstruktor für zirkulären Pufferspeicher
     * @param maximale Größe des Pufferspeichers (Anzahl der Elemente)
     */
    CircularBuffer(unsigned int maxElements);
    /**
     * @name Destruktor
     */
    ~CircularBuffer(void);

    /**
     * @name neues Element dem Pufferspeicher hinzufügen
     * @param Element vom Typ des Templates
     */
    void addElement(T& elem);

    /**
     * @name maximale Anzahl der Elemente im Pufferspeicher zurückgeben
     */
    unsigned int getMaxElementCount(void) { return maxElements; }

    /**
     * @name nächsten "freien" Index zurückgeben
     * @brief Gibt die nächste "freie" (d.h. processed=false) Indexposition im Puffer zurück.
     */
    unsigned int getNextIndex(void) { 
      std::unique_lock<std::mutex> lck(mtx);
      return nextIndex;
    }

    /**
     * @name Anzahl "freier" (d.h. processed=false) Elemente zurückgeben
     */
    unsigned int getElementCount(void) {
      std::unique_lock<std::mutex> lck(mtx);
      return availableElements;
    }

    /**
     * @name Sucht nach dem nächsten "freien" Element
     * @param eine Referenz vom Typ des Templates
     * @retval true, wenn ein freies Element vorhanden, andernfalls false
     */
    bool getElement(T& arg);

};
}

#endif
