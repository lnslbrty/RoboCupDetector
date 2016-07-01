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
    BlobDetectorFactory(unsigned int numThreads);
    ~BlobDetectorFactory();

    bool openCamera(void) { return cam->open(); }
    void closeCamera(void) { return cam->release(); }
    rc::Camera * getCamera(void) const { return cam; }

    void startThreads(void);
    void stopThreads(void);
    bool grabCameraImage(void);
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
    std::thread overwatch;

    static rc::Camera * cam;
#ifdef USE_XWINDOW
    static rc::Window * win;
#endif
    rc::CircularBuffer<cv::Mat> * cBuf;
#ifdef ENABLE_VIDEO
    char* filename = nullptr;
    cv::VideoWriter * videoOut = nullptr;
    std::mutex videoMtx;
#endif
};
}

#endif
