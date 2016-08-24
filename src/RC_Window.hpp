/**
 * @file    RC_Window.hpp
 * @author  Toni Uhlig <matzeton@googlemail.com>
 * @date    25.05.2016
 * @version 1.0
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
class Window {

  public:
    Window(void);
    ~Window(void);

    void stop(void);

    void addImage(cv::Mat image);

    void run(void);

    bool imagesAvailable(void) { return !images.empty(); }
    size_t imagesQueueSize() { return images.size(); }

    void setXWindow(bool enableWin) {
      this->doXWin = enableWin;
    }

    bool isXWindow(void) { return this->doXWin; }

  private:
    void previewImage(cv::Mat image);

    void wait(void) { cv::waitKey(1); }

    std::atomic<bool> doSmth;
    std::atomic<bool> doXWin;
    std::queue<cv::Mat> images;
    std::mutex images_mtx;
    std::thread thrd;

};
}

#endif
