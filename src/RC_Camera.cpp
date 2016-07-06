#include "RC_Camera.hpp"


rc::Camera::Camera(void) : raspicam::RaspiCam_Cv() {
  this->set(CV_CAP_PROP_FORMAT, CV_8UC3);
  this->set(CV_CAP_PROP_SATURATION, 50);
  this->set(CV_CAP_PROP_GAIN, 90);
  this->set(CV_CAP_PROP_EXPOSURE, 0);
}

bool rc::Camera::getImage(cv::Mat& image) {
  if (this->grab()) {
    this->retrieve(image);
    return true;
  } else return false;
}
