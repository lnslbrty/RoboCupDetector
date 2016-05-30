/**
 * Date: 25.05.2016
 * Author: Toni Uhlig <matzeton@googlemail.com>
 */

#ifndef RC_CAMERA_H
#define RC_CAMERA_H 1

#include <raspicam/raspicam.h>
#include <raspicam/raspicam_cv.h>

#include <raspicam/raspicamtypes.h>


namespace rc {
class Camera : public raspicam::RaspiCam_Cv {

  public:
    Camera(void) : raspicam::RaspiCam_Cv() {
      this->set(CV_CAP_PROP_FORMAT, CV_8UC3);
      rcam.setExposure(raspicam::RASPICAM_EXPOSURE_OFF);
    }
    ~Camera(void) { }

    bool getImage(cv::Mat& image) {
      if (this->grab()) {
        this->retrieve(image);
        return true;
      } else return false;
    }

    unsigned int getWidth(void) { return rcam.getWidth(); }
    unsigned int getHeight(void) { return rcam.getHeight(); }
    raspicam::RASPICAM_FORMAT getFormat(void) { return rcam.getFormat(); }
    raspicam::RASPICAM_AWB getAWB(void) const { return rcam.getAWB(); }

  private:
    raspicam::RaspiCam rcam;

};
}

#endif
