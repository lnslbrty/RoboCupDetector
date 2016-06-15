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
    explicit Semaphore(unsigned int _maxCount) : maxCount(_maxCount), count(0) {
    }
    ~Semaphore(void) {
    }

    void produce(void) {
      std::unique_lock<std::mutex> lck(mtx);
      count++;
      cond.wait(lck);
    }

    void consumeAll(void) {
      while (count < maxCount)
        std::this_thread::sleep_for(std::chrono::microseconds(25));
      {
        std::unique_lock<std::mutex> lck(mtx);
        count = 0;
      }
      cond.notify_all();
    }

    void abortConsumer(void) { count = maxCount; }

};
}

#endif
