#include <unistd.h>
#include <stdint.h>

#include <ctime>
#include <iostream>
#include <iomanip>
#include <raspicam/raspicam_cv.h>

#include "RC_BlobDetectorFactory.hpp"

#if defined(USE_XWINDOW_FLTRD) && !defined(USE_XWINDOW)
#error "USE_XWINDOW_FLTRD needs USE_XWINDOW"
#endif


struct cmd_opts {
  unsigned long long int count;
  char* videoFile;
  bool useXWindow;
  bool showFiltered;
  unsigned int width;
  unsigned int height;
  uint8_t sat;
  uint8_t gain;
  int8_t exp;
};
#define UNIMPLEMENTED(feature) { fprintf(stderr, "%s: feature not implemented (%s)\n", argv[0], feature); exit(1); }



static void usage(char* arg0) {
  fprintf(stderr, "\n%s: usage\n"
                  "\t-n [count]     number of frame to capture\n"
                  "\t-s [sat]       set camera saturation [0..100]\n"
                  "\t-g [gain]      set camera gain [0..100]\n"
                  "\t-e [exp]       set exposure [1..100] default: auto\n"
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
                "\n", arg0);
}

int main (int argc,char **argv) {
  time_t timer_begin,timer_end;
  struct cmd_opts opts = { 100,0,0,0,640,480,50,90,-1 };

  char c;
  while ((c = getopt(argc, argv, "n:s:g:e:v:xfw:h:p")) != -1) {
    if (c == 0xFF) break;
    switch (c) {

      case 'n':
        opts.count     = strtoul(optarg, NULL, 10);
        break;
      /************************/
      case 's':
        opts.sat       = strtoul(optarg, NULL, 10);
        break;
      /************************/
      case 'g':
        opts.gain      = strtoul(optarg, NULL, 10);
        break;
      /************************/
      case 'e':
        opts.exp       = strtoul(optarg, NULL, 10);
        break;
      /************************/
      case 'v':
#ifdef ENABLE_VIDEO
        opts.videoFile = strdup(optarg);
#else
        UNIMPLEMENTED("ENABLE_VIDEO");
#endif
        break;
      /************************/
      case 'x':
#ifdef USE_XWINDOW
        opts.useXWindow = true;
#else
        UNIMPLEMENTED("USE_XWINDOW");
#endif
        break;
      /***********************/
      case 'f':
#ifdef USE_XWINDOW_FLTRD
        opts.showFiltered = true;
#else
        UNIMPLEMENTED("USE_XWINDOW_FLTRD");
#endif
        break;
      /***********************/
      case 'w':
#if defined(USE_XWINDOW) || defined(ENABLE_VIDEO)
        opts.width = strtoul(optarg, NULL, 10);
#else
        UNIMPLEMENTED("USE_XWINDOW || ENABLE_VIDEO");
#endif
        break;
      /**********************/
      case 'h':
#if defined(USE_XWINDOW) || defined(ENABLE_VIDEO)
        opts.height = strtoul(optarg, NULL, 10);
#else
        UNIMPLEMENTED("USE_XWINDOW || ENABLE_VIDEO");
#endif
        break;
      /**********************/
      case 'p':
      default:
        usage(argv[0]);
        exit(1);
    }
  }

  rc::BlobDetectorFactory detector(4);
  detector.getCamera()->setSaturation(opts.sat);
  detector.getCamera()->setGain(opts.gain);
  detector.getCamera()->setExposure(opts.exp);
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
  auto cam = detector.getCamera();
  std::cout <<"Resolution: "<<cam->getWidth()<<"x"<<cam->getHeight()<<std::endl
            <<"Format....: "<<cam->getFormat()<<std::endl
            <<"Saturation: "<<cam->getSaturation()<<std::endl
            <<"Gain......: "<<cam->getGain()<<std::endl
            <<"Exposure..: "<<cam->getExposure()<<std::endl
            <<"------------------------------"<<std::endl;

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
  for (unsigned int i = 0; i < opts.count; ++i) {
    while (!detector.grabCameraImage())
      std::this_thread::yield();
    if (i%11 == 1) {
      time(&timer_end);
      secondsElapsed = difftime(timer_end,timer_begin);
      std::cout <<"\r["<<std::fixed<<std::setprecision(0)<<std::setw(7)<<secondsElapsed<<"] "
                  <<"["<<std::setw(6)<<i<<"] "
                  <<"["<<std::setw(3)<<std::setprecision(0)<<(float)(secondsElapsed > 0 ? (float)(opts.count/secondsElapsed) : 0.0)<<"] "
                  <<detector.outInfo()<<std::flush;
    }
  }
  std::cout <<std::endl;
  detector.stopThreads();
  cam->release();

  std::cout <<"FPS: "<<std::setprecision(10)<<(float)(opts.count/secondsElapsed)<<std::endl;
  return 0;
}
