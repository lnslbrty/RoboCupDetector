#include "RC_BlobDetectorFactory.hpp"

#ifdef USE_XWINDOW
rc::Window * rc::BlobDetectorFactory::win = nullptr;
#endif


rc::BlobDetectorFactory::BlobDetectorFactory(unsigned int numThreads) : rc::BlobDetector(), rc::Camera(numThreads*2) {
  this->numThreads = numThreads;
  thrds = new std::thread[numThreads];

  sema = new Semaphore(numThreads);
#ifdef USE_XWINDOW
  if (win == nullptr)
    win = new rc::Window();
#endif
}

rc::BlobDetectorFactory::~BlobDetectorFactory() {
  delete[] thrds;
  delete sema;
#ifdef USE_XWINDOW
  if (win != nullptr)
    delete win;
#endif
}

void rc::BlobDetectorFactory::startThreads(void) {
#ifdef USE_XWINDOW
  std::cout << "Open X11 preview window ...."<<std::endl;
  win->run();
#endif
#ifdef ENABLE_VIDEO
  if (filename != nullptr) {
    videoOut = new cv::VideoWriter(filename, CV_FOURCC('M','J','P','G'), 10, cv::Size(640,480), true);
    if (!videoOut->isOpened())
      std::cerr << "Could not open video file"<<std::endl;
  }
#endif

  doLoop = true;
  for (unsigned int i = 0; i < numThreads; ++i) {
    thrds[i] = std::thread([this](int num) {
      while (doLoop) {
        cv::Mat image;
        bool ret = false;
        ret = this->getElement(image);
        if (ret && !image.empty()) {
          cv::Mat filteredImage = process(image);
#if defined(USE_XWINDOW) || defined(ENABLE_VIDEO)
          cv::Size size(this->width, this->height);
          cv::Mat resImage;
          cv::resize(image, resImage, size);
#ifdef USE_XWINDOW
          if (win->isXWindow())
            win->addImage(rc::IMG_ORIGINAL, resImage);
#endif
#ifdef USE_XWINDOW_FLTRD
          if (win->isXWindowFltrd()) {
            cv::Mat resFilteredImage;
            cv::resize(filteredImage, resFilteredImage, size);
            win->addImage(rc::IMG_FILTERED, resFilteredImage);
          }
#endif
#endif
#ifdef ENABLE_VIDEO
          if (filename != nullptr) {
            videoMtx.lock();
            videoOut->write(resImage);
            videoMtx.unlock();
          }
#endif
        }
        sema->Synchronize();
      }
    }, i);
  }
}

void rc::BlobDetectorFactory::stopThreads(void) {
  doLoop = false;
  for (unsigned int i = 0; i < numThreads; ++i) {
    thrds[i].join();
  }
#ifdef USE_XWINDOW
  while (win->imagesAvailable()) {
    std::cout << "\r"<<outInfo()<<std::flush;
    std::this_thread::sleep_for(std::chrono::microseconds(250));
  }
  std::cout << "\r"<<outInfo()<<std::endl;
  win->stop();
#endif
#ifdef ENABLE_VIDEO
  if (filename) {
    videoMtx.lock();
    videoOut->release();
    delete videoOut;
    videoMtx.unlock();
  }
#endif
}

std::string rc::BlobDetectorFactory::outInfo(void) {
  std::stringstream out;
  out << "[CirularBufferPos: "<<this->getNextIndex()
      << " availableElements: "<<this->getElementCount();
#ifdef USE_XWINDOW
  if (win != nullptr)
    out << " ImageQueueSize: "<<win->imagesQueueSize();
#endif
  out << "]";
  return out.str();
}

