#ifndef RC_SEMAPHORE_H
#define RC_SEMAPHORE_H 1

#include <thread>
#include <mutex>
#include <condition_variable>


namespace rc {
class Semaphore {

  private:
    std::mutex mtx;
    std::condition_variable cond;
    unsigned int count;

  public:
    Semaphore(void) : count() { }
    ~Semaphore(void);

    void consume(void) {
      std::unique_lock<std::mutex> lck(mtx);
      while (count <= 0)
        cond.wait(lck);
    }

    void produce(void) {
      std::unique_lock<std::mutex> lck(mtx);
      count++;
    }

};
}

#endif
