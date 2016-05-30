/**
 * Date: 25.05.2016
 * Author: Toni Uhlig <matzeton@googlemail.com>
 */

#ifndef RC_BLOBDETECTORFACTORY_H
#define RC_BLOBDETECTORFACTORY_H 1

#include <raspicam/raspicam_cv.h>
#include <condition_variable>
#include <thread>

#include "RC_Camera.hpp"
#include "RC_CircularBuffer.hpp"
#include "RC_BlobDetector.hpp"


namespace rc {
class BlobDetectorFactory : public rc::BlobDetector {

  public:
    BlobDetectorFactory(unsigned int numThreads) {
      doSmth = true;
      this->numThreads = numThreads;
      thrds = new std::thread[numThreads];

      cam = new rc::Camera();
      cbuf = new rc::CircularBuffer<cv::Mat>(numThreads);
    }
    ~BlobDetectorFactory() {
      delete[] thrds;
      delete cbuf;
      delete cam;
    }

    bool openCamera(void) { return cam->open(); }
    void closeCamera(void) { return cam->release(); }
    rc::Camera * getCamera(void) { return cam; }

    void startThread(void) {
      for (unsigned int i = 0; i < numThreads; ++i) {
        thrds[i] = std::thread([this](int num) {
          std::cout << "Thread "<<num<<" started .. ";

          while (doSmth) {
            std::cout << "standby("<<num<<")"<<std::endl;

            if (cbuf_mtx.try_lock() == true) {
              /* TODO: do critical stuff */
              cbuf_mtx.unlock();
              /* TODO: do non-critical stuff */
            } else std::this_thread::sleep_for(std::chrono::microseconds(100));
          }
        }, i);
      }
    }

    bool processCurrentCameraImage(void) {
      cv::Mat image;
      if (cam->getImage(image)) {
        cbuf_mtx.lock();
        cbuf->addElement(image);
        cbuf_mtx.unlock();
        process(image);
      } else return false;
      return true;
    }

  private:
    std::mutex cbuf_mtx;

    static bool doSmth;
    static unsigned int numThreads;
    static std::thread * thrds;

    static rc::Camera * cam;
    static rc::CircularBuffer<cv::Mat> * cbuf;
};
}

#endif
