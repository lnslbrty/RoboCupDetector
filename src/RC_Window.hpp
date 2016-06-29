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
    Window(void) {
      doSmth = true;
      cv::namedWindow("cam-original", CV_WINDOW_NORMAL);
      wait();
#ifdef SHOW_FILTERED_IMAGE
      cv::namedWindow("cam-filtered", CV_WINDOW_NORMAL);
#endif
      wait();
    }
    ~Window(void) {
      if (doSmth)
        stop();
    }

    void stop(void) {
      images_mtx.lock();
      doSmth = false;
      images_mtx.unlock();
      thrd.join();
      cv::destroyAllWindows();
    }

    void addImage(enum imageType type, cv::Mat image) {
      struct imageObj obj;
      obj.image = image;
      obj.type = type;
      images_mtx.lock();
      images.push(obj);
      images_mtx.unlock();
    }

    void run(void) {
      thrd = std::thread([this](void) {
        while (1) {
          if (images_mtx.try_lock()) {
            if (!doSmth) {
              images_mtx.unlock();
              break;
            }
            if (images.empty()) {
              images_mtx.unlock();
              std::this_thread::sleep_for(std::chrono::microseconds(500));
              continue;
            }
            struct imageObj obj = images.front();
            previewImage(obj.type, obj.image);
            wait();
            images.pop();
            images_mtx.unlock();
          } else std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
      });
    }

    bool imagesAvailable(void) { return !images.empty(); }
    size_t imagesQueueSize() { return images.size(); }

  private:
    void previewImage(enum imageType itype, cv::Mat image) {
      switch (itype) {
        case IMG_ORIGINAL:
          cv::imshow("cam-original", image);
          break;

        case IMG_FILTERED:
#ifdef SHOW_FILTERED_IMAGE
          cv::imshow("cam-filtered", image);
#endif
          break;
      }
    }

    void wait(void) {
      cv::waitKey(1);
    }

    bool doSmth;
    std::queue<struct imageObj> images;
    std::mutex images_mtx;
    std::thread thrd;

};
}

#endif
