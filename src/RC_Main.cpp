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
  unsigned int width;           /** Bildbreite */
  unsigned int height;          /** Bildhöhe */
  unsigned int maxFps;          /** maximale FPS der Kamera */
  uint8_t sat;                  /** Farbsättigung */
  uint8_t gain;                 /** Bildverstärkung */
  int8_t exp;                   /** Belichtungsdauer */
  uint8_t thrds;                /** RoboCup Bilderkennungs- Threads */
  uint8_t cvthrds;              /** OpenCV Bildbearbeitungs- Thread */
};

/** Dieses Feature ist zur Zeit nicht im "binary" enthalten. Um das Feature zu integrieren muss das Projekt mit `cmake` bzw. `ccmake` neu kofniguriert/kompiliert/gelinkt werden. */
#define UNIMPLEMENTED(feature) { fprintf(stderr, "%s: feature not implemented (%s)\n", argv[0], feature); exit(1); }

/** Standartwerte für Konfigurationsvariablen der oben beschriebenen Datenstruktur */
static struct cmd_opts opts = { 0,100,0,0,640,480,20,50,90,-1,4,2 };


/**
 * @name Kommandozeilenhilfe
 * @brief Eine kurze Übersicht über alle verfügbaren Konfigurationsvariablen
 */
static void usage(char* arg0) {
  fprintf(stderr, "\n%s: usage\n"
                  "\t-S             start as daemon (fails if already started)\n"
                  "\t-K             kill the daemon\n"
                  "\t-n [count]     number of frame to capture [0..n] default: %llu\n"
                  "\t                 (0 means indefinitly capturing)\n"
                  "\t-s [sat]       set camera saturation [0..100] default: %u\n"
                  "\t-g [gain]      set camera gain [0..100] default: %u\n"
                  "\t-e [exp]       set exposure [-1..100] default: %d\n"
                  "\t-t [num]       set robocup thread count default: %u\n"
                  "\t-c [num]       set opencv thread count default: %u\n"
                  "\t-w [width]     base image width in pixels [0..n] default: %u\n"
                  "\t-h [height]    base image height in pixels [0..n] default: %u\n"
                  "\t-f [num]       maximal camera frames-per-second [1..n] default: %u\n"
#ifdef ENABLE_VIDEO
                  "VIDEO options:\n"
                  "\t-v [file]      save RIFF-avi stream to [file}\n"
#endif
#ifdef USE_XWINDOW
                  "XWINDOW options (X11 required):\n"
                  "\t-x             render images\n"
#endif
                  "\t-p             this\n"
#ifdef DEBUG
                  "\t-d             print OpenCV build/hardware information\n"
#endif
                "\n", arg0, opts.count, opts.sat, opts.gain, opts.exp, opts.thrds, opts.cvthrds, opts.width, opts.height, opts.maxFps);
}

/**
 * @name Debuginformationen ausgeben
 */
#ifdef DEBUG
static void print_opencv_info(void) {
  std::cout <<"OpenCV DEBUG"<<std::endl
            <<"[Build Information]"<<std::endl<<cv::getBuildInformation()<<std::endl
            <<"[HW Support]"<<std::endl
            <<"CPUs........: "<<cv::getNumberOfCPUs()<<std::endl
            <<"MMX.........: "<<cv::checkHardwareSupport(CV_CPU_MMX)<<std::endl
            <<"SSE.........: "
              <<(cv::checkHardwareSupport(CV_CPU_SSE) == true ? "SSE " : "")
              <<(cv::checkHardwareSupport(CV_CPU_SSE2) == true ? "SSE2 " : "")
              <<(cv::checkHardwareSupport(CV_CPU_SSE3) == true ? "SSE3 " : "")
              <<(cv::checkHardwareSupport(CV_CPU_SSSE3) == true ? "SSSE3 " : "")<<std::endl;
}
#endif

/**
 * @name  `main` Funktion
 * @brief  Diese wird vom Betriebssystem während des Programmstartes als Einsprungspunkt genutzt
 * @retval ganzzahliger Rückgabewert der z.B. auf der Shell ausgewertet werden kann
 */
int main (int argc,char **argv) {
  time_t timer_begin,timer_end;

  char c;
  while ((c = getopt(argc, argv, "SKn:s:g:e:t:c:v:xw:h:f:pd")) != -1) {
    if (c == 0xFF) break;
    switch (c) {

      case 'S':
        opts.daemonize = true;
        break;
      /*######################*/
      case 'K':
        return (rc::Daemon::KillByPidfile(PIDFILE, LOCKFILE) == true ? 0 : 1);
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
      case 't':
        opts.thrds     = strtoul(optarg, NULL, 10);
        break;
      /*######################*/
      case 'c':
        opts.cvthrds   = strtoul(optarg, NULL, 10);
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
      case 'w':
        opts.width = strtoul(optarg, NULL, 10);
        break;
      /*####################*/
      case 'h':
        opts.height = strtoul(optarg, NULL, 10);
        break;
      /*####################*/
      case 'f':
        opts.maxFps = strtoul(optarg, NULL, 10);
        break;
      /*####################*/
      case 'd':
#ifdef DEBUG
        print_opencv_info();
#else
        UNIMPLEMENTED("DEBUG");
#endif
        exit(1);
        break;
      /*####################*/
      case 'p':
      default:
        usage(argv[0]);
        exit(1);
    }
  }

  /* Anwendung als Diesnt im Hintergrund starten (falls aktiviert) */
  if (opts.daemonize == true) {
    if (rc::Daemon::instance()->Daemonize(PIDFILE, LOCKFILE, CHUSER) == false)
      return 1;
  } else {
    rc::setSignals();
  }

  /* Initialisierung (Kamera, OpenCV, etc) */
  rc::BlobDetectorFactory detector(opts.thrds);
  detector.setSaturation(opts.sat);
  detector.setGain(opts.gain);
  detector.setExposure(opts.exp);
  detector.setMaxFPS(opts.maxFps);
#ifdef USE_XWINDOW
  /* Vorschaufenster Optionen setzen */
  detector.getXWindow()->setXWindow( opts.useXWindow );
#endif
#ifdef ENABLE_VIDEO
  /* Ausgabedatei des Videos festlegen */
  detector.setVideoOut(opts.videoFile);
#endif
  /* Basis Bildgröße (in Pixel) festlegen */
  detector.setDimensions(opts.width, opts.height);

  /* Kamera initialisieren */
  if (!detector.openCamera()) {
    std::cerr << "Error opening the camera"<<std::endl;
    return -1;
  }
  /* Thread Anzahl (OpenCV Threads -> zur Zeit nicht verfügbar) */
  cv::setNumThreads(opts.cvthrds);
  /* optimierten OpenCV code ausführen (bei Raspberrypi nicht notwendig da kein mmx oder sse vorhanden */
  cv::setUseOptimized(true);

  /* ein paar Konfigurationsvariablen ausgeben */
  std::cout <<"Resolution..: "<<detector.getWidth()<<"x"<<detector.getHeight()<<std::endl
            <<"Format......: "<<detector.getFormat()<<std::endl
            <<"Max-FPS.....: "<<detector.getMaxFPS()<<std::endl
            <<"Saturation..: "<<detector.getSaturation()<<std::endl
            <<"Gain........: "<<detector.getGain()<<std::endl
            <<"Exposure....: "<<detector.getExposure()<<std::endl
            <<"OpenCV-Thrds: "<<cv::getNumThreads()<<std::endl
            <<"Optimised...: "<<cv::useOptimized()<<std::endl
            <<"Threads.....: "<<static_cast<int>(opts.thrds)<<std::endl;
#ifdef ENABLE_VIDEO
  if (opts.videoFile)
    std::cout <<"Saving RIFF-avi stream to file: "<<opts.videoFile<<std::endl;
#endif
#ifdef USE_XWINDOW
  if (opts.useXWindow)
    std::cout <<"Open X11 preview window (after processing)"<<std::endl;
#endif

  /* Startzeit feststellen (für FPS Berechnung) */
  time(&timer_begin);
  /* Arbeiter- Threads starten */
  detector.startThreads();
  /* Spaltenberzeichnung ausgeben */
  std::cout <<"------------------------------"<<std::endl
            <<"[Elapsed] [Images] [FPS] [BufferInfo]"<<std::endl;

  double secondsElapsed = 0.0f;
  unsigned int i = 0, n = 1;
  /* Hauptschleife */
  while (i <= opts.count && !rc::isDaemonTerminate()) {
    /* Kamerabild holen */
    while (!detector.grabCameraImage()) {
      std::this_thread::yield();
      continue;
    }
    if (!opts.daemonize && (n%3 == 1 || n == opts.count)) {
      /* vergangen Zeit merken (für FPS Berechnung) */
      time(&timer_end);
      secondsElapsed = difftime(timer_end,timer_begin);
      /* Informationen ausgeben (u.a. FPS) */
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
  /* Arbeiter- Threads stoppen */
  detector.stopThreads();
  /* Kamera deinitialisieren */
  detector.closeCamera();
  /* durchschnittliche FPS ausgeben */
  std::cout <<"FPS: "<<std::setprecision(10)<<(float)(n/secondsElapsed)<<std::endl;
  return 0;
}
