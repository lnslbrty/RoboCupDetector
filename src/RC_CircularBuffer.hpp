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

/** Klasse für _ein_ Element innerhalb des Puffers */
template <class T>
struct cbElement {
  /** ein Element (Template) */
  T element;
  /** bereits zurückgegebenes Element */
  bool processed;
  /** Zeitstempel (Wann wurde Element hinzugefügt?) */
  time_t tstamp;
};

/** Klasse für zirkulären Pufferspeicher */
template <class T>
class CircularBuffer {

  private:
    std::mutex mtx;                 /** Mutex für Vector */
    struct cbElement<T> * cBuffer;  /** Array der Länge `maxElements` */
    unsigned int nextIndex;         /** Feldindex, welcher beim nächsten `addElement(...)` überschrieben wird */
    unsigned int maxElements;       /** maximale Größe des Puffers */
    unsigned int availableElements; /** Anzahl "frischer" Einträge */
    unsigned int skippedElements;   /** Anzahl verworfener Einträge, da Zeitstempel zu alt */
    time_t maxAge;                  /** maximales Alter eines Eintrages (wird verworfen/nicht bearbeitet) */

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
    bool addElement(T& elem);

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
     * @name Anzahl übersprungener (d.h. Zeistempel zu alt) Elemente zurückgeben
     */
    unsigned int getSkipped(void) {
      std::unique_lock<std::mutex> lck(mtx);
      return skippedElements;
    }

    /**
     * @name Sucht nach dem nächsten "freien" Element
     * @param eine Referenz vom Typ des Templates
     * @retval true, wenn ein freies Element vorhanden, andernfalls false
     */
    bool getElement(T& arg);

    /**
     * @name Gibt Informationen über den Puffer aus
     * @retval Zeichenkette mit Pufferinfo
     */
    std::string getInfo(void);

};
}

#endif
