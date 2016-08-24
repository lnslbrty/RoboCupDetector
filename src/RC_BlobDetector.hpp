/**
 * @file    RC_BlobDetector.hpp
 * @author  Toni Uhlig <matzeton@googlemail.com>
 * @date    25.05.2016
 * @version 1.0
 */

#ifndef RC_BLOBDETECTOR_H
#define RC_BLOBDETECTOR_H 1

#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>


namespace rc {

/** zu filternde Farbe */
enum roboColor {
  RB_YELLOW, RB_BLUE
};

class BlobDetector {

  public:
    /**
     * @name Konstruktor
     */
    BlobDetector() {
    }

    /**
     * @name Destruktor
     */
    ~BlobDetector() {
    }

    /**
     * @name Farbfilter
     * @param Quellbild
     * @param zu filternde Farbe
     * @retval die gefilterte OpenCV Matrix
     */
    cv::Mat filterColor(cv::Mat src, enum roboColor rcol) {
      cv::Mat tmp, result;
      // konvertieren von RGB zu HSV
      cv::cvtColor(src, tmp, CV_BGR2HSV);
      // filtere HSV- Farbraum entsprechend der angegebenen roboColor
      switch (rcol) {
        case RB_YELLOW: /* GELB */
          cv::inRange(tmp, cv::Scalar(0, 180, 145), cv::Scalar(50, 255, 255), result);
          break;
        case RB_BLUE:   /* BLAU */
          cv::inRange(tmp, cv::Scalar(70, 30, 30), cv::Scalar(120, 255, 255), result);
          break;
      }
      return result;
    }

    /**
     * @name Sucht nach Konturen in einem Bild
     * @param Das zu filternde Bild.
     * @param die zu filternde Farbe
     * @retval Matrix nach dem Farbfilter
     */
    cv::Mat process(cv::Mat& image, enum roboColor rc);

};
}

#endif
