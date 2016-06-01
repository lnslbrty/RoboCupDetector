/**
 * @file
 * @author Toni Uhlig <matzeton@googlemail.com>
 * @date 25.05.2016
 * @version 1.0
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

    /**
     * Filtert einen bestimmten Farbbereich eines Bildes.
     *
     * @param Das zu filternde Bild.
     * @param Startfarbe
     * @param Endfarbe
     * @return Das gefilterte Bild.
     */
    cv::Mat filterByColorRange(cv::Mat& image, cv::Scalar start, cv::Scalar end) {
      cv::Mat mask;
      cv::inRange(image, start, end, mask);
      return mask;
    }

    /**
     * Filtert bestimmte Farbbereiche eines Bildes zur weiterverarbeitung.
     *
     * @param Das zu filternde Bild.
     * @return Das Bild mit angewendeten Farbfiltern.
     */
    cv::Mat process(cv::Mat& image) {
      /* Filter für rote Objekte */
      cv::Mat mask1 = this->filterByColorRange(image, cv::Scalar(0, 0, 0), cv::Scalar(70, 70, 255));
      cv::Mat mask2 = this->filterByColorRange(image, cv::Scalar(0, 0, 0), cv::Scalar(90, 90, 90));
      cv::Mat mask3 = this->filterByColorRange(image, cv::Scalar(220, 220, 220), cv::Scalar(255, 255, 255));
      /* TODO: Filter für gelbe/blaue Objekte */
      return mask1 & ~mask2 & ~mask3;
    }

    /**
     * Sucht nach Linien in einem Bild.
     *
     * @param Ein Bild (möglichst mit einem Farbfilter versehen).
     */
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
