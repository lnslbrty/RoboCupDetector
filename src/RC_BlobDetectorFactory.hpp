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

    void startThreads(void);
    void stopThreads(void);

    bool grabCameraImage(void) {
      bool ret = false;
      cv::Mat image;
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
