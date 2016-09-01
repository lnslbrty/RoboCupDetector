/**
 * @file    RC_Semaphore.hpp
 * @author  Toni Uhlig <matzeton@googlemail.com>
 * @date    16.07.2016
 * @version 1.0
 */

#ifndef RC_SEMAPHORE_H
#define RC_SEMAPHORE_H 1

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>


namespace rc {
class Semaphore {

  private:
    std::mutex mtx;               /** Semaphore Mutex für Condition */
    std::condition_variable cond; /** Semaphore Condition */
    unsigned int maxCount;        /** maximale Anzahl Konsumeinheiten */
    unsigned int count;           /** aktuelle Anzahl der Konsumeinheiten */

  public:
    /**
     * @name Semaphore Konstruktor
     * @param maximale Anzahl der Konsumeinheiten
     */
    explicit Semaphore(unsigned int _maxCount) : maxCount(_maxCount), count(_maxCount) {
    }
    /**
     * @name Destruktor
     */
    ~Semaphore(void) {
    }

    /**
     * @name Alle aufrufenden Threads blockieren, bis `count` == 0 ist
     * @brief Jeder Thread, der diese Methode aufruft, wird blockiert bis `count` == 0 ist
     */
    void Synchronize(void) {
      std::unique_lock<std::mutex> lck(mtx);
      count--;
      if (count == 0) {
        count = maxCount;
        cond.notify_all();
        return;
      }
      cond.wait(lck);
    }

    void NotifyAll(void) {
      cond.notify_all();
    }

};
}

#endif
