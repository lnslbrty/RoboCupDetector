#include <unistd.h>

#include <ctime>
#include <iostream>
#include <raspicam/raspicam_cv.h>

#include "RC_Camera.hpp"
#include "RC_CircularBuffer.hpp"
#include "RC_BlobDetector.hpp"
#ifdef USE_XWINDOW
#include "RC_Window.hpp"
#endif

using namespace std;

 
int main ( int argc,char **argv ) {
  rc::CircularBuffer<cv::Mat> cbuf(10);
  rc::Camera cam;
  rc::BlobDetector detector;

  time_t timer_begin,timer_end;
  cv::Mat image;
  int nCount=100;
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

  // Open camera
  if (!cam.open()) {
    cerr << "Error opening the camera"<<endl;
    return -1;
  }

  cout << "Resolution: "<<cam.getWidth()<<"x"<<cam.getHeight()<<std::endl
       << "Format....: "<<cam.getFormat()<<std::endl
       << "AWB-Mode..: "<<cam.getAWB()<<std::endl;

  // Start capture
  cout << std::endl<<"Capturing "<<nCount<<" frames ...."<<endl;
  time ( &timer_begin );

#ifdef USE_XWINDOW
  cout << "Open X11 preview window ...."<<endl;
  rc::setupWindow();
#endif

  for ( int i=0; i<nCount; i++ ) {
    if (!cam.getImage(image))
      cerr << "capture failed"<<endl;
    if (i%13 == 1) {
      cout << "\r captured "<<i<<" images, circular buffer at pos "<<cbuf.getNextIndex()<<std::flush;
    }
    std::stringstream ss;
    ss << "raspicam_cv_image_"<<i<<".jpg";
    // save images on disk?
    if (saveImages)
      cv::imwrite(ss.str(), image);
    // save images in our circular buffer
    cbuf.addElement(image);

    if (cbuf.getElement(image)) {
      detector.process(image);
      //detector.detectLines();
#ifdef USE_XWINDOW
      rc::previewImage(image);
      rc::previewFilteredImage(detector.getFilteredImage());
      rc::wait();
#endif
    } else cout << "Buffer error"<<endl;
  }

  // stop camera
  cam.release();

  // show time statistics
  time( &timer_end ); /* get current time; same as: timer = time(NULL)  */
  double secondsElapsed = difftime( timer_end,timer_begin );
  cout << std::endl<<secondsElapsed<<" seconds for "<< nCount<<"  frames : FPS = "<<  ( float ) ( ( float ) ( nCount ) /secondsElapsed ) <<endl;
}
