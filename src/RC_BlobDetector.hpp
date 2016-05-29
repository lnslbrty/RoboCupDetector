/**
 * Date: 25.05.2016
 * Author: Toni Uhlig <matzeton@googlemail.com>
 */

#ifndef RC_BLOBDETECTOR_H
#define RC_BLOBDETECTOR_H 1

#define MAX_DETECTIONS 50

#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>


namespace rc {
class BlobDetector {

  public:
    BlobDetector() {
    }

    cv::Mat filterByColorRange(cv::Scalar start, cv::Scalar end) {
      cv::Mat mask;
      cv::inRange(this->HSV, start, end, mask);
      return mask;
    }

    void process(cv::Mat& mat) {
      cv::cvtColor(mat, this->HSV, cv::COLOR_BGR2HSV);

      cv::Mat mask1 = this->filterByColorRange(cv::Scalar(0, 70, 50), cv::Scalar(10, 255, 255));
      cv::Mat mask2 = this->filterByColorRange(cv::Scalar(170, 70, 50), cv::Scalar(180, 255, 255));
      this->FilteredImage = mask1 | mask2;
    }

    cv::Mat& getHSV(void) { return HSV; }
    cv::Mat& getFilteredImage(void) { return FilteredImage; }

  private:
    cv::Mat HSV;
    cv::Mat FilteredImage;

};
}

#endif
