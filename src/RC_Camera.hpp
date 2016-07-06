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
    Camera(void);
    ~Camera(void) { }

    bool getImage(cv::Mat& image);

    unsigned int getWidth(void) { return this->get(CV_CAP_PROP_FRAME_WIDTH); }
    unsigned int getHeight(void) { return this->get(CV_CAP_PROP_FRAME_HEIGHT); }
    raspicam::RASPICAM_FORMAT getFormat(void) { return static_cast<raspicam::RASPICAM_FORMAT>(this->get(CV_CAP_PROP_FORMAT)); }

};
}

#endif
