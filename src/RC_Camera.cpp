#include "RC_Camera.hpp"


rc::Camera::Camera(unsigned int maxBufferElements) : raspicam::RaspiCam_Cv(), rc::CircularBuffer<cv::Mat>(maxBufferElements) {
  this->set(CV_CAP_PROP_FORMAT, CV_8UC3);
}

bool rc::Camera::grabCameraImage(void) {
  bool ret = false;
  cv::Mat image;
  if (this->getElementCount() < this->getMaxElementCount())
  {
    if (this->grab()) {
      this->retrieve(image);
      this->addElement(image);
      ret = true;
    }
  }
  return ret;
}

