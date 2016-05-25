#include <ctime>
#include <iostream>
#include <raspicam/raspicam_cv.h>

#include "RC_Camera.hpp"
#include "RC_CircularBuffer.hpp"

using namespace std;

 
int main ( int argc,char **argv ) {
  rc::CircularBuffer<cv::Mat> cbuf(10);
  rc::Camera cam;

  time_t timer_begin,timer_end;
  cv::Mat image;
  int nCount=100;

  //Open camera
  cout << "Opening Camera..."<<endl;
  if (!cam.open()) {
    cerr << "Error opening the camera"<<endl;
    return -1;
  }

  //Start capture
  cout << "Capturing "<<nCount<<" frames ...."<<endl;
  time ( &timer_begin );

  for ( int i=0; i<nCount; i++ ) {
    cam.getImage(image);
    if ( i%5==0 ) cout << "\r captured "<<i<<" images"<<std::flush;
    std::stringstream ss;
    ss << "raspicam_cv_image_"<<i<<".jpg";
    cv::imwrite(ss.str(), image);
  }

  cout<<"Stop camera..."<<endl;
  cam.release();
  //show time statistics
  time( &timer_end ); /* get current time; same as: timer = time(NULL)  */
  double secondsElapsed = difftime( timer_end,timer_begin );
  cout << secondsElapsed<<" seconds for "<< nCount<<"  frames : FPS = "<<  ( float ) ( ( float ) ( nCount ) /secondsElapsed ) <<endl;
  //save image 
  cv::imwrite("raspicam_cv_image.jpg",image);
  cout<<"Image saved at raspicam_cv_image.jpg"<<endl;
}
