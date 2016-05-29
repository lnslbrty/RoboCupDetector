/**
 * Date: 28.05.2016
 * Author: Toni Uhlig (matzeton@googlemail.com)
 */

#ifndef RC_WINDOW_H
#define RC_WINDOW_H 1

#include <raspicam/raspicam_cv.h>


namespace rc {

  inline void setupWindow(void) {
    cv::namedWindow("cam-original", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("cam-filtered", CV_WINDOW_AUTOSIZE);
  }

  inline void previewImage(cv::Mat& image) {
    cv::imshow("cam-original", image);
  }

  inline void previewFilteredImage(cv::Mat& image) {
    cv::imshow("cam-filtered", image);
  }

  inline void wait(void) {
    cv::waitKey(1);
  }

}

#endif
