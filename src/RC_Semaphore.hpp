#ifndef RC_SEMAPHORE_H
#define RC_SEMAPHORE_H 1

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>


namespace rc {
class Semaphore {

  private:
    std::mutex mtx;
    std::condition_variable cond;
    unsigned int maxCount, count;

  public:
    explicit Semaphore(unsigned int _maxCount) : maxCount(_maxCount), count(_maxCount) {
    }
    ~Semaphore(void) {
    }

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

};
}

#endif
