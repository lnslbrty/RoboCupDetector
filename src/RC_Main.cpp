/**
 * @file   RC_Main.cpp
 * @date   20.05.2016
 * @author Toni Uhlig <matzeton@googlemail.com>
 * @brief  Initialisierung, Parameter auswerten, Einstellungen setzen
 */

#include <unistd.h>
#include <stdint.h>

#include <ctime>
#include <iostream>
#include <iomanip>
#include <raspicam/raspicam_cv.h>

#include "RC_Daemon.hpp"
#include "RC_BlobDetectorFactory.hpp"

#if defined(USE_XWINDOW_FLTRD) && !defined(USE_XWINDOW)
#error "USE_XWINDOW_FLTRD needs USE_XWINDOW"
#endif

/** Standartpfad für die ProzessID- Datei (daemon Modus) */
#ifndef PIDFILE
#define PIDFILE "/var/run/robocup.pid"
#endif

/** Standartpfad für Sperr- Datei (daemon Modus) */
#ifndef LOCKFILE
#define LOCKFILE "/var/lock/robocup.lock"
#endif

/** Standartuser, um nicht benötigte Privilegien zu "droppen" (daemon Modus) */
#ifndef CHUSER
#define CHUSER "robocup"
#endif


/** Datenstruktur für Kommandozeilen Parameter */
struct cmd_opts {
  bool daemonize;               /** Daemonmodus */
  unsigned long long int count; /** Anzahl der auszuwertenden Bilder */
  char* videoFile;              /** Pfad zur Videodatei (falls mit ENABLE_VIDEO kompiliert */
  bool useXWindow;              /** Vorschaubild rendern? (benötigt USE_XWINDOW) */
  bool showFiltered;            /** gefiltertes Vorschaubild rendern? (benötigt USE_XWINDOW_FLTRD) */
  unsigned int width;           /** Bildbreite */
  unsigned int height;          /** Bildhöhr */
  uint8_t sat;                  /** Farbsättigung */
  uint8_t gain;                 /** Bildverstärkung */
  int8_t exp;                   /** Belichtungsdauer */
};

/** Dieses Feature ist zur Zeit nicht im "binary" enthalten. Um das Feature zu integrieren muss das Projekt mit `cmake` bzw. `ccmake` neu kofniguriert/kompiliert/gelinkt werden. */
#define UNIMPLEMENTED(feature) { fprintf(stderr, "%s: feature not implemented (%s)\n", argv[0], feature); exit(1); }


static struct cmd_opts opts = { 0,100,0,0,0,640,480,50,90,-1 };


static void usage(char* arg0) {
  fprintf(stderr, "\n%s: usage\n"
                  "\t-S             start as daemon (fails if already started)\n"
                  "\t-K             kill the daemon\n"
                  "\t-n [count]     number of frame to capture [1..n] default: %llu\n"
                  "\t-s [sat]       set camera saturation [0..100] default: %u\n"
                  "\t-g [gain]      set camera gain [0..100] default: %u\n"
                  "\t-e [exp]       set exposure [-1..100] default: %d\n"
#ifdef ENABLE_VIDEO
                  "VIDEO options:\n"
                  "\t-v [file]      save RIFF-avi stream to [file}\n"
#endif
#ifdef USE_XWINDOW
                  "XWINDOW options (X11 required):\n"
                  "\t-x             render images\n"
#ifdef USE_XWINDOW_FLTRD
                  "\t-f             render filtered image\n"
#endif
#endif
#if defined(USE_XWINDOW) || defined(ENABLE_VIDEO)
                  "VIDEO/XWINDOW options:\n"
                  "\t-w [width]     output image width in pixels\n"
                  "\t-h [heiight]   output image height in pixels\n"
#endif
                  "\t-p             this\n"
                "\n", arg0, opts.count, opts.sat, opts.gain, opts.exp);
}


/**
 * @name  `main` Funktion
 * @brief  Diese wird vom Betriebssystem während des Programmstartes als Einsprungspunkt genutzt
 * @retval ganzzahliger Rückgabewert der z.B. auf der Shell ausgewertet werden kann
 */
int main (int argc,char **argv) {
  time_t timer_begin,timer_end;

  char c;
  while ((c = getopt(argc, argv, "SKn:s:g:e:v:xfw:h:p")) != -1) {
    if (c == 0xFF) break;
    switch (c) {

      case 'S':
        opts.daemonize = true;
        break;
      /*######################*/
      case 'K':
        return rc::Daemon::KillByPidfile(PIDFILE);
      /*######################*/
      case 'n':
        opts.count     = strtoul(optarg, NULL, 10);
        break;
      /*######################*/
      case 's':
        opts.sat       = strtoul(optarg, NULL, 10);
        break;
      /*######################*/
      case 'g':
        opts.gain      = strtoul(optarg, NULL, 10);
        break;
      /*######################*/
      case 'e':
        opts.exp       = strtoul(optarg, NULL, 10);
        break;
      /*######################*/
      case 'v':
#ifdef ENABLE_VIDEO
        opts.videoFile = strdup(optarg);
#else
        UNIMPLEMENTED("ENABLE_VIDEO");
#endif
        break;
      /*######################*/
      case 'x':
#ifdef USE_XWINDOW
        opts.useXWindow = true;
#else
        UNIMPLEMENTED("USE_XWINDOW");
#endif
        break;
      /*#####################*/
      case 'f':
#ifdef USE_XWINDOW_FLTRD
        opts.showFiltered = true;
#else
        UNIMPLEMENTED("USE_XWINDOW_FLTRD");
#endif
        break;
      /*#####################*/
      case 'w':
#if defined(USE_XWINDOW) || defined(ENABLE_VIDEO)
        opts.width = strtoul(optarg, NULL, 10);
#else
        UNIMPLEMENTED("USE_XWINDOW || ENABLE_VIDEO");
#endif
        break;
      /*####################*/
      case 'h':
#if defined(USE_XWINDOW) || defined(ENABLE_VIDEO)
        opts.height = strtoul(optarg, NULL, 10);
#else
        UNIMPLEMENTED("USE_XWINDOW || ENABLE_VIDEO");
#endif
        break;
      /*####################*/
      case 'p':
      default:
        usage(argv[0]);
        exit(1);
    }
  }

  if (opts.daemonize == true) {
    if (rc::Daemon::instance()->Daemonize(PIDFILE, LOCKFILE, CHUSER) == false)
      return 1;
  } else {
    rc::setSignals();
  }
  
  rc::BlobDetectorFactory detector(4);
  detector.setSaturation(opts.sat);
  detector.setGain(opts.gain);
  detector.setExposure(opts.exp);
#ifdef USE_XWINDOW
  detector.getXWindow()->setXWindow( opts.useXWindow, opts.showFiltered );
#endif
#ifdef ENABLE_VIDEO
  detector.setVideoOut(opts.videoFile);
#endif

  if (!detector.openCamera()) {
    std::cerr << "Error opening the camera"<<std::endl;
    return -1;
  }
#if defined(USE_XWINDOW) || defined(ENABLE_VIDEO)
  detector.setDimensions(opts.width, opts.height);
#endif
  std::cout <<"Resolution: "<<detector.getWidth()<<"x"<<detector.getHeight()<<std::endl
            <<"Format....: "<<detector.getFormat()<<std::endl
            <<"Saturation: "<<detector.getSaturation()<<std::endl
            <<"Gain......: "<<detector.getGain()<<std::endl
            <<"Exposure..: "<<detector.getExposure()<<std::endl
            <<"------------------------------"<<std::endl;
  std::cout <<"OpenCV settings"<<std::endl
            <<"Threads...: "<<cv::getThreadNum()<<std::endl;

#ifdef ENABLE_VIDEO
  if (opts.videoFile)
    std::cout <<"Saving RIFF-avi stream to file: "<<opts.videoFile<<std::endl;
#endif
#ifdef USE_XWINDOW
  if (opts.useXWindow)
    std::cout <<"Open X11 preview window (after processing)"<<std::endl;
  if (opts.showFiltered)
    std::cout <<"Open X11 preview window (filtered image)"<<std::endl;
#endif
  time(&timer_begin);
  detector.startThreads();
  std::cout <<"------------------------------"<<std::endl
            <<"[Elapsed] [Images] [FPS] [BufferInfo]"<<std::endl;

  double secondsElapsed = 0.0f;
  unsigned int i = 0, n = 1;
  while (i <= opts.count && !rc::isDaemonTerminate()) {
    while (!detector.grabCameraImage()) {
      std::this_thread::yield();
      continue;
    }
    if (!opts.daemonize && (n%3 == 1 || n == opts.count)) {
      time(&timer_end);
      secondsElapsed = difftime(timer_end,timer_begin);
      std::cout <<"\r["<<std::fixed<<std::setprecision(0)<<std::setw(7)<<secondsElapsed<<"] "
                  <<"["<<std::setw(6)<<n<<"] "
                  <<"["<<std::setw(3)<<std::setprecision(0)<<(float)(secondsElapsed > 0 ? (float)(n/secondsElapsed) : 0.0)<<"] "
                  <<detector.outInfo()<<std::flush;
    }
    if (opts.count > 0)
      i++;
    n++;
  }
  std::cout <<std::endl;
  detector.stopThreads();
  detector.closeCamera();

  std::cout <<"FPS: "<<std::setprecision(10)<<(float)(n/secondsElapsed)<<std::endl;
  return 0;
}
