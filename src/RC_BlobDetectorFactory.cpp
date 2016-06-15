#include "RC_BlobDetectorFactory.hpp"

rc::Camera * rc::BlobDetectorFactory::cam = nullptr;
#ifdef USE_XWINDOW
rc::Window * rc::BlobDetectorFactory::win = nullptr;
#endif


void rc::BlobDetectorFactory::startThreads(void) {
#ifdef USE_XWINDOW
  std::cout << "Open X11 preview window ...."<<std::endl;
  win = new rc::Window();
  win->run();
#endif
#ifdef ENABLE_VIDEO
  videoOut = new cv::VideoWriter("./out.avi", CV_FOURCC('M','J','P','G'), 10, cv::Size(640,480), true);
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
        if (!doSmth)
          break;
        cv::Mat image;
        bool ret = false;
        /* TODO: do critical stuff */
        ret = cBuf->getElement(image);
        /* TODO: do non-critical stuff */
        if (ret && !image.empty()) {
          cv::Mat filteredImage = process(image);
          filteredImage = detectLines(filteredImage);
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
    std::cout << "Thread "<<num<<" stopped"<<std::endl;
    }, i);
  }
}

void rc::BlobDetectorFactory::stopThreads(void) {
#ifdef USE_XWINDOW
  while (win->imagesAvailable()) {
    std::cout << "\r"<<outInfo()<<std::flush;
    std::this_thread::sleep_for(std::chrono::microseconds(250));
  }
  std::cout << std::endl;
#endif
  doSmth = false;
#ifdef USE_XWINDOW
  delete win;
#endif
  for (unsigned int i = 0; i < numThreads; ++i) {
    thrds[i].detach();
  }
  sema->abortConsumer();
  overwatch.join();
#ifdef ENABLE_VIDEO
  videoOut->release();
  delete videoOut;
#endif
}

