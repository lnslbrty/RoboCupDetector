/**
 * @file    RC_Threads.hpp
 * @author  Toni Uhlig <matzeton@googlemail.com>
 * @date    04.09.2016
 * @version 1.0
 */

#ifndef RC_THREADS_H
#define RC_THREADS_H 1

#include <thread>
#include <atomic>
#include <functional>


namespace rc {

/** Standart "Rückruf"-Funktionstyp für Threads */
typedef std::function<void (unsigned int threadNum)> threadFunc_t;

/** Abstraktionsschicht für C++11-Threads (implementiert als Threadpool) */
class Threads {

  public:
    /**
     * @name Threadpool Konstruktor
     * @param [in] Anzahl der zu "Arbeiter"- Threads
     */
    Threads(unsigned int numThreads);

    /**
     * @name Standart Destruktor
     */
    ~Threads(void);

    /**
     * @name alle Threads starten
     */
    void startThreads(void);

    /**
     * @name alle Threads stoppen und auf das Ende warten
     */
    void stopThreads(void);

    /**
     * @name den Threadpräfix setzen
     * @param [in] Name als Zeichenkette
     */
    void setThreadNamePrefix(std::string prefixName) { thrdPrefix = prefixName; }

    /**
     * @name die Threadspezifische "Rückruf"-Funktion setzen
     * @param [in] Threadfunktion vom Typ threadFunc_t (siehe Header)
     */
    void setThreadFunction(threadFunc_t thrdFunc) { this->thrdFunc = thrdFunc; }

    /**
     * @name die Threadspezifische Initialisierungs-Funktion setzen
     * @param [in] Threadfunktion vom Typ threadFunc_t (siehe Header)
     */
    void setInitFunction(threadFunc_t initFunc) { this->initFunc = initFunc; }

    /**
     * @name die Threadspezifische Aufräum-Funktion setzen
     * @param [in] Threadfunktion vom Typ threadFunc_t (siehe Header)
     */
    void setCleanupFunction(threadFunc_t cleanupFunc) { this->cleanupFunc = cleanupFunc; }

    /**
     * @name Anzahl der Threads zurückgeben
     * @retval Threadanzahl
     */
    unsigned int getNumThreads(void) { return this->numThreads; }

  private:
    unsigned int numThreads;            /** Anzahl der Arbeiter- Threads */
    std::string thrdPrefix;             /** Threadname Präfix */
    std::thread * thrds = nullptr;      /** Datenstruktur fuer Threads als Feld der Groese numThreads */
    std::atomic_bool doLoop;            /** Hauptschleife der Arbeiter- Threads aktiv */
    threadFunc_t thrdFunc = nullptr;    /** Thread-Callback-Funktion */
    threadFunc_t initFunc = nullptr;    /** Initialisierungs-Callback-Funktion */
    threadFunc_t cleanupFunc = nullptr; /** Aufräum-Callback-Funktion */

};
}

#endif
