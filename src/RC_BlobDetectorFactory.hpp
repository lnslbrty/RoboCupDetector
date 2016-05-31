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
#ifdef USE_XWINDOW
#include "RC_Window.hpp"
#endif


namespace rc {
class BlobDetectorFactory : public rc::BlobDetector {

  public:
    BlobDetectorFactory(unsigned int numThreads) {
      this->numThreads = numThreads;
      thrds = new std::thread[numThreads];
      filteredImages = new cv::Mat[numThreads];

      cam = new rc::Camera();
      cBuf = new rc::CircularBuffer<cv::Mat>(numThreads);
    }
    ~BlobDetectorFactory() {
      delete[] thrds;
      delete[] filteredImages;
      delete cBuf;
      delete cam;
    }

    bool openCamera(void) { return cam->open(); }
    void closeCamera(void) { return cam->release(); }
    rc::Camera * getCamera(void) const { return cam; }
    cv::Mat getFilteredImage(unsigned int i) { return filteredImages[i]; }
    bool hasFilteredImage(unsigned int i) { return !filteredImages[i].empty(); }

    cv::Mat getImage(void) {
      cBuf_mtx.lock();
      cv::Mat image = cBuf->getCurrentElement();
      cBuf_mtx.unlock();
      return image;
    }

    void startThreads(void) {
#ifdef USE_XWINDOW
      std::cout << "Open X11 preview window ...."<<std::endl;
      rc::setupWindow();
#endif
      for (unsigned int i = 0; i < numThreads; ++i) {
        thrds[i] = std::thread([this](int num) {
          std::cout << "Thread "<<num<<" started .. "<<std::endl;

          while (1) {
            if (cBuf_mtx.try_lock() == true) {
              if (!doSmth) {
                cBuf_mtx.unlock();
                break;
              }
              cv::Mat image;
              bool ret = false;
              /* TODO: do critical stuff */
              ret = cBuf->getElement(image);
              cBuf_mtx.unlock();
              /* TODO: do non-critical stuff */
              if (!ret)
                continue;
              if (!image.empty()) {
                process(image);
#ifdef USE_XWINDOW
                win_mtx.lock();
                rc::previewImage(image);
                if (hasFilteredImage(num))
                  rc::previewFilteredImage(getFilteredImage(num));
                rc::wait();
                win_mtx.unlock();
#endif
              }
            } else std::this_thread::sleep_for(std::chrono::microseconds(100));
          }
          std::cout << "Thread "<<num<<" stopped"<<std::endl;
        }, i);
      }
    }

    void stopThreads(void) {
      cBuf_mtx.lock();
      doSmth = false;
      cBuf_mtx.unlock();
#ifdef USE_XWINDOW
      win_mtx.lock();
      rc::closeWindows();
      win_mtx.unlock();
#endif
      for (unsigned int i = 0; i < numThreads; ++i) {
        thrds[i].join();
        cBuf_mtx.unlock();
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
    std::mutex cBuf_mtx;
#ifdef USE_XWINDOW
    std::mutex win_mtx;
#endif

    bool doSmth;
    unsigned int numThreads;
    std::thread * thrds;

    static rc::Camera * cam;
    rc::CircularBuffer<cv::Mat> * cBuf;
    cv::Mat * filteredImages;
};
}

#endif
