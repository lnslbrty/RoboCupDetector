#include <iomanip>

#include "RC_DetectorFactory.hpp"


#ifdef USE_XWINDOW
rc::Window * rc::BlobDetectorFactory::win = nullptr;
#endif


rc::BlobDetectorFactory::BlobDetectorFactory(unsigned int numThreads) : rc::BlobDetector(), rc::Camera(numThreads*2) {
  /* Threadpool initialisieren */
  this->threads = new rc::Threads(numThreads);
#ifdef USE_XWINDOW
  /* X11 Vorschaufenster instanziieren */
  if (win == nullptr)
    win = new rc::Window();
#endif
}

rc::BlobDetectorFactory::~BlobDetectorFactory(void) {
#ifdef USE_XWINDOW
  if (win != nullptr)
    delete win;
#endif
}

void rc::BlobDetectorFactory::startThreads(void) {
  data = new threadData[threads->getNumThreads()];

  threads->setThreadFunction([this](unsigned int threadNum) {
      cv::Mat image;
      bool ret = false;
      /* Bild aus dem Pufferspeicher holen */
      ret = this->getElement(image);
      if (ret && !image.empty()) {
        /* Bild analysieren (für GELBE und BLAUE Blobs) */
        cv::Mat origImageY = image.clone();
        cv::Mat filteredImageY = process(origImageY, RB_YELLOW, *data[threadNum].piY, *data[threadNum].tc);
        cv::Mat origImageB = image.clone();
        cv::Mat filteredImageB = process(origImageB, RB_BLUE, *data[threadNum].piB, *data[threadNum].tc);
        image = origImageY | origImageB;
#ifdef ENABLE_HTTPD
        /* Bilder dem microhttpd bereitstellen */
        if (this->httpd != nullptr) {
          httpd->setImage(IMAGE_DRAWED, image);
          httpd->setImage(IMAGE_FILTERED_YELLOW, filteredImageY);
          httpd->setImage(IMAGE_FILTERED_BLUE, filteredImageB);
        }
#endif
#ifdef USE_XWINDOW
        /* X11 Vorschaubild anzeigen */
        if (win->isXWindow())
          win->addImage(image);
#endif
#ifdef ENABLE_VIDEO
        /* Bild in den Videodatenstrom schreiben */
        if (filename != nullptr) {
          videoMtx.lock();
          videoOut->write(image);
          videoMtx.unlock();
        }
#endif
      }
    });

  threads->setInitFunction([this](unsigned int threadNum) {
      data[threadNum].piY = new processed_image();
      data[threadNum].piB = new processed_image();
      data[threadNum].tc  = new time_consumption();
    });

  threads->setCleanupFunction([this](unsigned int threadNum) {
      out.lock();
      std::cout <<"Thread("<<threadNum<<") time consumption:"<<std::endl
                <<"\tcolor filter: "<<std::setprecision (5)<<data[threadNum].tc->avg_color<<std::endl
#ifdef ENABLE_BLUR
                <<"\tblur........: "<<std::setprecision (5)<<data[threadNum].tc->avg_blur<<std::endl
#endif
                <<"\tcontours....: "<<std::setprecision (5)<<data[threadNum].tc->avg_contours<<std::endl
                <<"\tdraw........: "<<std::setprecision (5)<<data[threadNum].tc->avg_draw<<std::endl;
      out.unlock();
      delete data[threadNum].piY;
      delete data[threadNum].piB;
      delete data[threadNum].tc;
    });

#ifdef USE_XWINDOW
  win->run();
#endif
#ifdef ENABLE_VIDEO
  /* Videodatenstrom initialisieren und zum schreiben in Ausgabedatei vorbereiten */
  if (filename != nullptr) {
    videoOut = new cv::VideoWriter(filename, CV_FOURCC('M','J','P','G'), 10, cv::Size(this->width, this->height), true);
    if (!videoOut->isOpened())
      std::cerr << "Could not open video file"<<std::endl;
  }
#endif

  threads->setThreadNamePrefix("robocup");
  threads->startThreads();
}

void rc::BlobDetectorFactory::stopThreads(void) {
  threads->stopThreads();
  delete data;
  data = nullptr;
#ifdef USE_XWINDOW
  while (win->imagesAvailable()) {
    std::cout << "\r"<<outInfo()<<std::flush;
    std::this_thread::sleep_for(std::chrono::microseconds(250));
  }
  std::cout << "\r"<<outInfo()<<std::endl;
  win->stop();
#endif
#ifdef ENABLE_VIDEO
  /* Videodatenstrom schließen und Speicher freigeben */
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
  out << "[" << this->getInfo();
#ifdef USE_XWINDOW
  if (win != nullptr)
    out << " ImageQueueSize: "<<win->imagesQueueSize();
#endif
  out << "]";
  return out.str();
}

