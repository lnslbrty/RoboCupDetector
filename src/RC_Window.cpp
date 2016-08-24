#include "RC_Window.hpp"


rc::Window::Window(void) {
  doSmth = true;
  doXWin = false;
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

void rc::Window::addImage(cv::Mat image) {
  images_mtx.lock();
  images.push(image);
  images_mtx.unlock();
}

void rc::Window::run(void) {
  if (isXWindow())
    cv::namedWindow("cam-original", CV_WINDOW_NORMAL);
  wait();

  thrd = std::thread([this](void) {
    while (doSmth) {
      if (images_mtx.try_lock()) {
        if (images.empty()) {
          images_mtx.unlock();
          std::this_thread::yield();
          continue;
        }
        cv::Mat image = images.front();
        previewImage(image);
        wait();
        images.pop();
        images_mtx.unlock();
      } else std::this_thread::yield();
    }
  });
}

void rc::Window::previewImage(cv::Mat image) {
  cv::imshow("cam-original", image);
}

