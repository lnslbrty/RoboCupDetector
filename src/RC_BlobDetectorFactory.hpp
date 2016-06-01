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

    cv::Mat getImage(void) {
      cBuf_mtx.lock();
      cv::Mat image = cBuf->getCurrentElement();
      cBuf_mtx.unlock();
      return image;
    }

    void startThreads(void) {
#ifdef USE_XWINDOW
      std::cout << "Open X11 preview window ...."<<std::endl;
      win = new rc::Window();
      win->run();
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
                cv::Mat filteredImage = process(image);
#ifdef USE_XWINDOW
                win->addImage(rc::IMG_ORIGINAL, image);
                win->addImage(rc::IMG_FILTERED, filteredImage);
#endif
              }
            } else std::this_thread::sleep_for(std::chrono::microseconds(100));
          }
          std::cout << "Thread "<<num<<" stopped"<<std::endl;
        }, i);
      }
    }

    void stopThreads(void) {
#ifdef USE_XWINDOW
      while (win->imagesAvailable()) {
        std::cout << "\r"<<outInfo()<<"  "<<std::flush;
        std::this_thread::sleep_for(std::chrono::microseconds(250));
      }
      std::cout << std::endl;
#endif
      cBuf_mtx.lock();
      doSmth = false;
      cBuf_mtx.unlock();
#ifdef USE_XWINDOW
      delete win;
      win = nullptr;
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

    std::string outInfo(void) {
      std::stringstream out;
      out << "[CirularBufferPos: "<<cBuf->getNextIndex();
#ifdef USE_XWINDOW
      if (win != nullptr)
        out << " ImageQueueSize: "<<win->imagesStackSize();
#endif
      out << "]";
      return out.str();
    }

  private:
    std::mutex cBuf_mtx;

    bool doSmth;
    unsigned int numThreads;
    std::thread * thrds;

    static rc::Camera * cam;
#ifdef USE_XWINDOW
    static rc::Window * win;
#endif
    rc::CircularBuffer<cv::Mat> * cBuf;
};
}

#endif
