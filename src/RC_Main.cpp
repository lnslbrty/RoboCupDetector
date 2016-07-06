#include <unistd.h>

#include <ctime>
#include <iostream>
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
};
#define UNIMPLEMENTED(feature) { fprintf(stderr, "%s: eature not implemented (%s)\n", argv[0], feature); exit(1); }


using namespace std;


static void usage(char* arg0) {
  fprintf(stderr, "\n%s: usage\n"
                  "\t-n [count]     number of frame to capture\n"
#ifdef ENABLE_VIDEO
                  "VIDEO options:\n"
                  "\t-v [file]      save image to [file}\n"
#endif
#ifdef USE_XWINDOW
                  "XWINDOW options:\n"
                  "\t-x             render images (needs X11)\n"
#ifdef USE_XWINDOW_FLTRD
                  "\t-f             render filtered image\n"
#endif
#endif
#if defined(USE_XWINDOW) || defined(ENABLE_VIDEO)
                  "VIDEO/XWINDOW options:\n"
                  "\t-w [width]     output image width in pixels\n"
                  "\t-h [heiight]   output image height in pixels\n"
#endif
                "\n", arg0);
}

int main (int argc,char **argv) {
  time_t timer_begin,timer_end;
  struct cmd_opts opts = { 100,0,0,0,640,480 };

  char c;
  while ((c = getopt(argc, argv, "n:v:xfw:h:")) != -1) {
    if (c == 0xFF) break;
    switch (c) {

      case 'n':
        opts.count = strtoul(optarg, NULL, 10);
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
      default:
        usage(argv[0]);
        exit(1);
    }
  }

  rc::BlobDetectorFactory detector(4);
#ifdef USE_XWINDOW
  detector.getXWindow()->setXWindow( opts.useXWindow, opts.showFiltered );
#endif
#ifdef ENABLE_VIDEO
  detector.setVideoOut(opts.videoFile);
#endif

  if (!detector.openCamera()) {
    cerr << "Error opening the camera"<<endl;
    return -1;
  }
#if defined(USE_XWINDOW) || defined(ENABLE_VIDEO)
  detector.setDimensions(opts.width, opts.height);
#endif
  auto cam = detector.getCamera();
  cout << "Resolution: "<<cam->getWidth()<<"x"<<cam->getHeight()<<std::endl
       << "Format....: "<<cam->getFormat()<<std::endl;

  cout << std::endl<<"Capturing "<<opts.count<<" frames ...."<<endl;
  time(&timer_begin);

  detector.startThreads();
  for (unsigned int i = 0; i < opts.count; ++i) {
    while (!detector.grabCameraImage())
      std::this_thread::yield();
    if (i%11 == 1) {
      cout << "\r captured "<<i<<" images "<<detector.outInfo()<<std::flush;
    }
  }
  cout << "\r captured "<<opts.count<<" images "<<detector.outInfo()<<std::flush;
  cout << endl;
  detector.stopThreads();

  cam->release();

  time(&timer_end);
  double secondsElapsed = difftime(timer_end,timer_begin);
  cout << endl<<secondsElapsed<<" seconds for "<<opts.count<<"  frames : FPS = "<<  ( float ) ( ( float ) (opts.count)/secondsElapsed ) <<endl;

  std::terminate();
  return 0;
}
