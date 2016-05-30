/**
 * Date: 25.05.2016
 * Author: Toni Uhlig <matzeton@googlemail.com>
 */

#ifndef RC_BLOBDETECTORFACTORY_H
#define RC_BLOBDETECTORFACTORY_H 1

#include <iostream>
#include <ostream>
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
      this->numThreads = numThreads;
      thrds = new std::thread[numThreads];

      cam = new rc::Camera();
      cBuf = new rc::CircularBuffer<cv::Mat>(numThreads);
    }
    ~BlobDetectorFactory() {
      delete[] thrds;
      delete cBuf;
      delete cam;
    }

    bool openCamera(void) { return cam->open(); }
    void closeCamera(void) { return cam->release(); }

    rc::Camera * getCamera(void) const { return cam; }

    cv::Mat getImage(void) const {
      cBuf_mtx.lock();
      cv::Mat image = cBuf->getCurrentElement();
      cBuf_mtx.unlock();
      return image;
    }

    void startThread(void) {
      for (unsigned int i = 0; i < numThreads; ++i) {
        thrds[i] = std::thread([this](int num) {
          std::cout << "Thread "<<num<<" started .. ";

          while (doSmth) {
            std::cout << "standby("<<num<<")"<<std::endl;

            if (cBuf_mtx.try_lock() == true) {
              cv::Mat image;
              /* TODO: do critical stuff */
              if (!cBuf->getElement(image))
                continue;
              cBuf_mtx.unlock();
              /* TODO: do non-critical stuff */
              process(image);
            } else std::this_thread::sleep_for(std::chrono::microseconds(100));
          }
        }, i);
      }
    }

    bool grabCameraImage(void) {
      bool ret = false;
      cv::Mat image;
      cBuf_mtx.lock();
      bool gotImage = cam->getImage(image);
      if (gotImage) {
        cBuf->addElement(image);
        ret = true;
      }
      cBuf_mtx.unlock();
      return ret;
    }

    std::ostream& outBufferInfo(std::ostream& out) {
      out << "";
      return out;
    }

  private:
    static std::mutex cBuf_mtx;

    static bool doSmth;
    static unsigned int numThreads;
    static std::thread * thrds;

    static rc::Camera * cam;
    static rc::CircularBuffer<cv::Mat> * cBuf;
};
}

#endif
