#include "RC_Camera.hpp"


rc::Camera::Camera(void) : raspicam::RaspiCam_Cv() {
  this->set(CV_CAP_PROP_FORMAT, CV_8UC3);
}

bool rc::Camera::getImage(cv::Mat& image) {
  if (this->grab()) {
    this->retrieve(image);
    return true;
  } else return false;
}
