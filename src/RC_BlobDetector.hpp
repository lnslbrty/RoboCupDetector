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

    cv::Mat filterByColorRange(cv::Mat& image, cv::Scalar start, cv::Scalar end) {
      cv::Mat mask;
      cv::inRange(image, start, end, mask);
      return mask;
    }

    cv::Mat process(cv::Mat& image) {
      /* Filter für rote Objekte */
      cv::Mat mask1 = this->filterByColorRange(image, cv::Scalar(0, 0, 0), cv::Scalar(70, 70, 255));
      cv::Mat mask2 = this->filterByColorRange(image, cv::Scalar(0, 0, 0), cv::Scalar(90, 90, 90));
      cv::Mat mask3 = this->filterByColorRange(image, cv::Scalar(220, 220, 220), cv::Scalar(255, 255, 255));
      return mask1 & ~mask2 & ~mask3;
    }

    void detectLines(cv::Mat& filteredImage) {
      std::vector<cv::Vec4i> lines;
      cv::HoughLinesP(filteredImage, lines, 1, CV_PI/180, 50, 50, 10);
      for (size_t i = 0; i < lines.size(); i++) {
        cv::Vec4i l = lines[i];
        cv::Point p1, p2;
        p1 = cv::Point(l[0], l[1]);
        p2 = cv::Point(l[2], l[3]);
        //calculate angle in radian,  if you need it in degrees just do angle * 180 / PI
        float angle = atan2(p1.y - p2.y, p1.x - p2.x);
        std::cout << "Angle: "<<angle<<std::endl;
      }
    }

};
}

#endif
