/**
 * Date: 28.05.2016
 * Author: Toni Uhlig (matzeton@googlemail.com)
 */

#ifndef RC_WINDOW_H
#define RC_WINDOW_H 1

#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <queue>


namespace rc {
enum imageType {
  IMG_ORIGINAL, IMG_FILTERED
};

struct imageObj {
  cv::Mat image;
  enum imageType type;
};

class Window {

  public:
    Window(void);
    ~Window(void);

    void stop(void);

    void addImage(enum imageType type, cv::Mat image);

    void run(void);

    bool imagesAvailable(void) { return !images.empty(); }
    size_t imagesQueueSize() { return images.size(); }

    void setXWindow(bool enableWin, bool enableFltrd) {
      this->doXWin = enableWin;
#ifdef USE_XWINDOW_FLTRD
      this->doXWinFltrd = enableFltrd;
#endif
    }

    bool isXWindow(void) { return this->doXWin; }
#ifdef USE_XWINDOW_FLTRD
    bool isXWindowFltrd(void) { return this->doXWinFltrd; }
#else
    bool isXWindowFltrd(void) { return false; }
#endif

  private:
    void previewImage(enum imageType itype, cv::Mat image);

    void wait(void) { cv::waitKey(1); }

    std::atomic<bool> doSmth;
    std::atomic<bool> doXWin;
#ifdef USE_XWINDOW_FLTRD
    std::atomic<bool> doXWinFltrd;
#endif
    std::queue<struct imageObj> images;
    std::mutex images_mtx;
    std::thread thrd;

};
}

#endif
