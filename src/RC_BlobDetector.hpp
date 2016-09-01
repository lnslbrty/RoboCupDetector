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
#include <opencv2/highgui/highgui.hpp>


namespace rc {

/** zu filternde Farbe */
enum roboColor {
  RB_YELLOW, RB_BLUE
};

struct time_consumption & getAvgTime(void);

struct time_consumption {
  float avg_color    = 0;
  float avg_blur     = 0;
  float avg_contours = 0;
  float avg_draw     = 0;
};

/** wichtige Informationen eines bearbeiteten Bildes in dieser Struktur speichern */
struct processed_image {
  cv::Point2f coords[4]; /** Positionen der vier Ecken */
  float angle;           /** Winkel des rotierenden Rechteckes */
  float distance;        /** die (geschätzte!) Entfernung zur Kamera */
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
          cv::inRange(tmp, cv::Scalar(0, 120, 105), cv::Scalar(50, 255, 255), result);
          break;
        case RB_BLUE:   /* BLAU */
          cv::inRange(tmp, cv::Scalar(80, 50, 60), cv::Scalar(120, 255, 255), result);
          break;
      }
      return result;
    }

    /**
     * @name Sucht nach Konturen in einem Bild
     * @param Das zu filternde Bild.
     * @param die zu filternde Farbe
     * @param ausgewertete Informationen in dieser Struktur speichern
     * @retval Matrix nach dem Farbfilter
     */
    cv::Mat process(cv::Mat& image, enum rc::roboColor rc, rc::processed_image& pi, rc::time_consumption& tc);

};
}

#endif
