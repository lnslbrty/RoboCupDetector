/**
 * Date: 25.05.2016
 * Author: Toni Uhlig <matzeton@googlemail.com>
 */

#ifndef RC_CAMERA_H
#define RC_CAMERA_H 1

#include <raspicam/raspicam_cv.h>


namespace rc {
class Camera : public raspicam::RaspiCam_Cv {

  public:
    Camera(void) : raspicam::RaspiCam_Cv() {
      this->set(CV_CAP_PROP_FORMAT, CV_8UC1);
    }

    bool getImage(cv::Mat& image) {
      if (this->grab()) {
        this->retrieve(image);
        return true;
      } else return false;
    }

};
}

#endif
