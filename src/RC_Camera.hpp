/**
 * Date: 25.05.2016
 * Author: Toni Uhlig <matzeton@googlemail.com>
 */

#ifndef RC_CAMERA_H
#define RC_CAMERA_H 1

#include <raspicam/raspicam.h>
#include <raspicam/raspicam_cv.h>
#include <raspicam/raspicamtypes.h>

#include "RC_CircularBuffer.hpp"


namespace rc {
class Camera : public raspicam::RaspiCam_Cv, public rc::CircularBuffer<cv::Mat> {

  public:
    Camera(unsigned int maxBufferElements);
    ~Camera(void) { }

    bool grabCameraImage(void);

    unsigned int getWidth(void) { return this->get(CV_CAP_PROP_FRAME_WIDTH); }
    unsigned int getHeight(void) { return this->get(CV_CAP_PROP_FRAME_HEIGHT); }
    raspicam::RASPICAM_FORMAT getFormat(void) { return static_cast<raspicam::RASPICAM_FORMAT>(this->get(CV_CAP_PROP_FORMAT)); }
    unsigned int getSaturation(void) { return this->get(CV_CAP_PROP_SATURATION); }
    unsigned int getGain(void) { return this->get(CV_CAP_PROP_GAIN); }
    int getExposure(void) { return this->get(CV_CAP_PROP_EXPOSURE); }

    void setSaturation(unsigned int value) { this->set(CV_CAP_PROP_SATURATION, value); }
    void setGain(unsigned int value) { this->set(CV_CAP_PROP_GAIN, value); }
    void setExposure(int value) { this->set(CV_CAP_PROP_EXPOSURE, value); }

};
}

#endif
