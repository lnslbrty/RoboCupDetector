#include "RC_BlobDetectorFactory.hpp"

rc::Camera * rc::BlobDetectorFactory::cam = nullptr;
#ifdef USE_XWINDOW
rc::Window * rc::BlobDetectorFactory::win = nullptr;
#endif


rc::BlobDetectorFactory::BlobDetectorFactory(unsigned int numThreads) {
  this->numThreads = numThreads;
  thrds = new std::thread[numThreads];

  cam = new rc::Camera();
  cBuf = new rc::CircularBuffer<cv::Mat>(numThreads*2);
  sema = new Semaphore(numThreads);
#ifdef USE_XWINDOW
  if (win == nullptr)
    win = new rc::Window();
#endif
}

rc::BlobDetectorFactory::~BlobDetectorFactory() {
  delete[] thrds;
  delete cBuf;
  delete cam;
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

  overwatch = std::thread([this](){
    while (1) { sema->produce(); std::this_thread::yield(); }
    std::cout << "Overwatch thread stopped"<<std::endl;
  });
  for (unsigned int i = 0; i < numThreads; ++i) {
    thrds[i] = std::thread([this](int num) {
      std::cout << "Thread "<<num<<" started .. "<<std::endl;

      while (1) {
        cv::Mat image;
        bool ret = false;
        ret = cBuf->getElement(image);
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
        sema->consume();
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
  std::cout << "\r"<<outInfo()<<std::endl;
  win->stop();
  delete win;
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

bool rc::BlobDetectorFactory::grabCameraImage(void) {
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

std::string rc::BlobDetectorFactory::outInfo(void) {
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

