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
      cv::inRange(this->mat, start, end, mask);
      return mask;
    }

    void process(cv::Mat& mat) {
      this->mat = mat;
      cv::Mat mask1 = this->filterByColorRange(cv::Scalar(0, 0, 0), cv::Scalar(80, 80, 255));
      cv::Mat mask2 = this->filterByColorRange(cv::Scalar(0, 0, 0), cv::Scalar(90, 90, 90));
      cv::Mat mask3 = this->filterByColorRange(cv::Scalar(220, 220, 220), cv::Scalar(255, 255, 255));
      this->FilteredImage = mask1 & ~mask2 & ~mask3;
    }

    cv::Mat& getMat(void) { return mat; }
    cv::Mat& getFilteredImage(void) { return FilteredImage; }

  private:
    cv::Mat mat;
    cv::Mat FilteredImage;

};
}

#endif
