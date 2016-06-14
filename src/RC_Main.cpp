#include <unistd.h>

#include <ctime>
#include <iostream>
#include <raspicam/raspicam_cv.h>

#include "RC_BlobDetectorFactory.hpp"


using namespace std;

 
int main (int argc,char **argv) {
  time_t timer_begin,timer_end;
  int nCount = 100;
  bool saveImages = false;

  // Parse command line arguments
  char c;
  if (argc > 1)
  while ((c = getopt(argc, argv, "sn:")) != -1) {
    if (c == 0xFF) break;
    switch (c) {
      case 's':
        saveImages = true;
        break;
      case 'n':
        nCount = strtoul(optarg, NULL, 10);
        break;
      default:
        cerr << "Usage: "<<argv[0]<<" -s | -n"<<endl;
        return 1;
    }
  }
  cout << "saveImages: "<<saveImages<<endl
       << "nCount....: "<<nCount<<endl;

  rc::BlobDetectorFactory detector(4);
  // Open camera
  if (!detector.openCamera()) {
    cerr << "Error opening the camera"<<endl;
    return -1;
  }

  auto cam = detector.getCamera();
  cout << "Resolution: "<<cam->getWidth()<<"x"<<cam->getHeight()<<std::endl
       << "Format....: "<<cam->getFormat()<<std::endl
       << "AWB-Mode..: "<<cam->getAWB()<<std::endl;

  // Start capture
  cout << std::endl<<"Capturing "<<nCount<<" frames ...."<<endl;
  time(&timer_begin);

  detector.startThreads();
  for (int i=0; i<nCount; i++) {
    while (!detector.grabCameraImage()) {
      std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
    if (i%11 == 1) {
      cout << "\r captured "<<i<<" images "<<detector.outInfo()<<std::flush;
    }
  }
  cout << "\r captured "<<nCount<<" images "<<detector.outInfo()<<std::flush;
  cout << endl;
  detector.stopThreads();

  // stop camera
  cam->release();

  // show time statistics
  time( &timer_end ); /* get current time; same as: timer = time(NULL)  */
  double secondsElapsed = difftime(timer_end,timer_begin);
  cout << endl<<secondsElapsed<<" seconds for "<< nCount<<"  frames : FPS = "<<  ( float ) ( ( float ) ( nCount ) /secondsElapsed ) <<endl;
}
