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

    unsigned int getWidth(void) { return camInfo.getWidth(); }
    unsigned int getHeight(void) { return camInfo.getHeight(); }
    raspicam::RASPICAM_FORMAT getFormat(void) { return camInfo.getFormat(); }
    raspicam::RASPICAM_AWB getAWB(void) const { return camInfo.getAWB(); }

  private:
    raspicam::RaspiCam camInfo;

};
}

#endif
