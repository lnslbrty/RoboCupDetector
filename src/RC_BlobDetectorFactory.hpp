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
#include <atomic>

#include "RC_Camera.hpp"
#include "RC_Semaphore.hpp"
#include "RC_CircularBuffer.hpp"
#include "RC_BlobDetector.hpp"
#ifdef USE_XWINDOW
#include "RC_Window.hpp"
#endif


namespace rc {
class BlobDetectorFactory : private rc::BlobDetector, public rc::Camera {

  public:
    BlobDetectorFactory(unsigned int numThreads);
    ~BlobDetectorFactory();

    bool openCamera(void) { return this->open(); }
    void closeCamera(void) { return this->release(); }

    void startThreads(void);
    void stopThreads(void);
    std::string outInfo(void);

#if defined(USE_XWINDOW) || defined(ENABLE_VIDEO)
    void setDimensions(unsigned int width, unsigned int height) { this->width = width; this->height = height; }
#endif

#ifdef ENABLE_VIDEO
    void setVideoOut(char* filename) { this->filename = filename; }
#endif

#ifdef USE_XWINDOW
    static rc::Window * getXWindow(void) { return win; }
#endif

  private:
#if defined(USE_XWINDOW) || defined(ENABLE_VIDEO)
    unsigned int width, height;
#endif

    Semaphore * sema = nullptr;
    unsigned int numThreads;
    std::thread * thrds = nullptr;
    std::atomic<bool> doLoop;

#ifdef USE_XWINDOW
    static rc::Window * win;
#endif
#ifdef ENABLE_VIDEO
    char* filename = nullptr;
    cv::VideoWriter * videoOut = nullptr;
    std::mutex videoMtx;
#endif
};
}

#endif
