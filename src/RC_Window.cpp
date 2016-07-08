#include "RC_Window.hpp"


rc::Window::Window(void) {
  doSmth = true;
  doXWin = false;
#ifdef USE_XWINDOW_FLTRD
  doXWinFltrd = false;
#endif
}


rc::Window::~Window(void) {
  if (doSmth) {
    stop();
  }
}

void rc::Window::stop(void) {
  doSmth = false;
  images_mtx.lock();
  cv::destroyAllWindows();
  thrd.join();
  images_mtx.unlock();
}

void rc::Window::addImage(enum imageType type, cv::Mat image) {
  struct imageObj obj;
  obj.image = image;
  obj.type = type;
  images_mtx.lock();
  images.push(obj);
  images_mtx.unlock();
}

void rc::Window::run(void) {
  if (isXWindow())
    cv::namedWindow("cam-original", CV_WINDOW_NORMAL);
  wait();
#ifdef USE_XWINDOW_FLTRD
  if (isXWindowFltrd())
    cv::namedWindow("cam-filtered", CV_WINDOW_NORMAL);
  wait();
#endif

  thrd = std::thread([this](void) {
    while (doSmth) {
      if (images_mtx.try_lock()) {
        if (images.empty()) {
          images_mtx.unlock();
          std::this_thread::yield();
          continue;
        }
        struct imageObj obj = images.front();
        previewImage(obj.type, obj.image);
        wait();
        images.pop();
        images_mtx.unlock();
      } else std::this_thread::yield();
    }
  });
}

void rc::Window::previewImage(enum imageType itype, cv::Mat image) {
  switch (itype) {
    case IMG_ORIGINAL:
      if (isXWindow())
        cv::imshow("cam-original", image);
      break;
    case IMG_FILTERED:
#ifdef USE_XWINDOW_FLTRD
      if (isXWindowFltrd())
        cv::imshow("cam-filtered", image);
#endif
      break;
  }
}

