#include <iomanip>

#include "RC_BlobDetectorFactory.hpp"


#ifdef USE_XWINDOW
rc::Window * rc::BlobDetectorFactory::win = nullptr;
#endif


rc::BlobDetectorFactory::BlobDetectorFactory(unsigned int numThreads) : rc::BlobDetector(), rc::Camera(numThreads*2) {
  this->numThreads = numThreads;
  /* Thread-Objekte instanziieren */
  thrds = new std::thread[numThreads];
  /* Semaphor-Objekt für Threads instanziieren */
  sema = new Semaphore(numThreads);
#ifdef USE_XWINDOW
  /* X11 Vorschaufenster instanziieren */
  if (win == nullptr)
    win = new rc::Window();
#endif
}

rc::BlobDetectorFactory::~BlobDetectorFactory() {
  /* Destruktoren aufrufen und Speicher freigeben */
  delete[] thrds;
  delete sema;
#ifdef USE_XWINDOW
  if (win != nullptr)
    delete win;
#endif
}

void rc::BlobDetectorFactory::startThreads(void) {
#ifdef ENABLE_HTTPD
  /* WebServer starten */
  std::cout <<"Starting WebServer (Port: 8080) .."<<std::endl;
  /* WebServer lauscht auf TCP-Port 8080 und zeigt maximal drei Bilder an */
  httpd = new rc::WebServer(8080, 3);
  httpd->start();
#endif
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

  doLoop = true;
  /* alle Threads erstellen + ausführem */
  for (unsigned int i = 0; i < numThreads; ++i) {
    /* Thread Einsprungsfunktion über Lambda zuweisen */
    thrds[i] = std::thread([this](int num) {
      /* benutzerdefinierten Threadnamen zuweisen */
      auto handle = thrds[num].native_handle();
      std::string pname("robocup worker");
      pname += std::to_string(num);
      pthread_setname_np(handle, pname.c_str());
      rc::processed_image * piY = new processed_image();
      rc::processed_image * piB = new processed_image();
      rc::time_consumption * tc = new time_consumption();
      /* Hauptschleife der Arbeiter- Threads */
      while (doLoop) {
        cv::Mat image;
        bool ret = false;
        /* Bild aus dem Pufferspeicher holen */
        ret = this->getElement(image);
        if (ret && !image.empty()) {
          /* Bild verkleinern, um CPU zu entlasten */
          cv::resize(image, image, cv::Size(this->width, this->height));
          /* Bild analysieren (für GELBE und BLAUE Blobs) */
          cv::Mat origImageY = image.clone();
          cv::Mat filteredImageY = process(origImageY, RB_YELLOW, *piY, *tc);
          cv::Mat origImageB = image.clone();
          cv::Mat filteredImageB = process(origImageB, RB_BLUE, *piB, *tc);
          image = origImageY | origImageB;
#ifdef ENABLE_HTTPD
          /* Bilder dem microhttpd bereitstellen */
          httpd->setImage(0, image);
          httpd->setImage(1, filteredImageY);
          httpd->setImage(2, filteredImageB);
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
        /* warten, bis alle Arbeiter- Threads fertig sind, bevor es weiter geht */
        sema->Synchronize();
      }
      out.lock();
      std::cout <<"Thread("<<num<<") time consumption:"<<std::endl
                <<"\tcolor filter: "<<std::setprecision (15)<<tc->avg_color<<std::endl
                <<"\tblur........: "<<std::setprecision (15)<<tc->avg_blur<<std::endl
                <<"\tcontours....: "<<std::setprecision (15)<<tc->avg_contours<<std::endl
                <<"\tdraw........: "<<std::setprecision (15)<<tc->avg_draw<<std::endl;
      out.unlock();
      delete piY;
      delete piB;
      delete tc;
    }, i);
  }
}

void rc::BlobDetectorFactory::stopThreads(void) {
  doLoop = false;
  /* auf das Ende aller Arbeiter- Threads warten */
  for (unsigned int i = 0; i < numThreads; ++i) {
    sema->NotifyAll();
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
  /* Videodatenstrom schließen und Speicher freigeben */
  if (filename) {
    videoMtx.lock();
    videoOut->release();
    delete videoOut;
    videoMtx.unlock();
  }
#endif
#ifdef ENABLE_HTTPD
  /* WebServer stoppen */
  httpd->stop();
  delete httpd;
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

