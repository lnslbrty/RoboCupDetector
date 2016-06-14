/**
 * Date: 25.05.2016
 * Author: Toni Uhlig <matzeton@googlemail.com>
 */

#ifndef RC_BLOBDETECTORFACTORY_H
#define RC_BLOBDETECTORFACTORY_H 1

#include <iostream>
#include <ostream>
#include <raspicam/raspicam_cv.h>
#include <thread>
#include <condition_variable>

#include "RC_Camera.hpp"
#include "RC_Semaphore.hpp"
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
      cBuf = new rc::CircularBuffer<cv::Mat>(numThreads*2);
      sema = new Semaphore(numThreads);
    }
    ~BlobDetectorFactory() {
      delete[] thrds;
      delete cBuf;
      delete cam;
      delete sema;
    }

    bool openCamera(void) { return cam->open(); }
    void closeCamera(void) { return cam->release(); }
    rc::Camera * getCamera(void) const { return cam; }

    void startThreads(void) {
#ifdef USE_XWINDOW
      std::cout << "Open X11 preview window ...."<<std::endl;
      win = new rc::Window();
      win->run();
#endif
#ifdef ENABLE_VIDEO
      videoOut = new cv::VideoWriter("./vout.avi", CV_FOURCC('M','J','P','G'), 10, cv::Size(640,480), true);
      if (!videoOut->isOpened()) {
        std::cout << "Could not open video file"<<std::endl;
      }
#endif
      doSmth = true;
      overwatch = std::thread([this](){ while (doSmth) { sema->consumeAll(); } });
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
              if (ret && !image.empty()) {
                cv::Mat filteredImage = process(image);
#if defined(USE_XWINDOW) || defined(ENABLE_VIDEO)
                cv::Size size(640, 480);
                cv::Mat resImage, resFilteredImage;
                cv::resize(image, resImage, size);
                cv::resize(filteredImage, resFilteredImage, size);
#endif
#ifdef USE_XWINDOW
                win->addImage(rc::IMG_ORIGINAL, resImage);
                win->addImage(rc::IMG_FILTERED, resFilteredImage);
#endif
#ifdef ENABLE_VIDEO
                videoMtx.lock();
                videoOut->write(resImage);
                videoMtx.unlock();
#endif
              }
              sema->produce();
            }
          }
          std::cout << "Thread "<<num<<" stopped"<<std::endl;
        }, i);
      }
    }

    void stopThreads(void) {
#ifdef USE_XWINDOW
      while (win->imagesAvailable()) {
        std::cout << "\r"<<outInfo()<<std::flush;
        std::this_thread::sleep_for(std::chrono::microseconds(250));
      }
      std::cout << std::endl;
#endif
      cBuf_mtx.lock();
      doSmth = false;
      cBuf_mtx.unlock();
#ifdef USE_XWINDOW
      delete win;
#endif
      for (unsigned int i = 0; i < numThreads; ++i) {
        thrds[i].join();
        cBuf_mtx.unlock();
      }
      overwatch.join();
#ifdef ENABLE_VIDEO
      videoOut->release();
      delete videoOut;
#endif
    }

    bool grabCameraImage(void) {
      bool ret = false;
      cv::Mat image;
      cBuf_mtx.lock();
      if (cBuf->getElementCount() < cBuf->getMaxElementCount())
#ifdef USE_XWINDOW
         if (win->imagesQueueSize() < cBuf->getMaxElementCount())
#endif
      {
        if (cam->getImage(image)) {
          cBuf->addElement(image);
          ret = true;
        }
      }
      cBuf_mtx.unlock();
      return ret;
    }

    std::string outInfo(void) {
      std::stringstream out;
      out << "[CirularBufferPos: "<<cBuf->getNextIndex()
          << " availableElements: "<<cBuf->getElementCount();
#ifdef USE_XWINDOW
      if (win != nullptr)
        out << " ImageQueueSize: "<<win->imagesQueueSize();
#endif
      out << "]";
      return out.str();
    }

  private:
    Semaphore * sema;
    std::mutex cBuf_mtx;

    bool doSmth;
    unsigned int numThreads;
    std::thread * thrds;
    std::thread overwatch;

    static rc::Camera * cam;
#ifdef USE_XWINDOW
    static rc::Window * win;
#endif
    rc::CircularBuffer<cv::Mat> * cBuf;
#ifdef ENABLE_VIDEO
    cv::VideoWriter * videoOut;
    std::mutex videoMtx;
#endif
};
}

#endif
