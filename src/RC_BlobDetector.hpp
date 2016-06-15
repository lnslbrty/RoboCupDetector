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
     * @param Startfarbe (nicht RGB, sondern im BGR Format)
     * @param Endfarbe (nicht RGB, sondern im BGR Format)
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
    cv::Mat process(cv::Mat& image);

    /**
     * Sucht nach Linien in einem Bild.
     *
     * @param Ein Bild (möglichst mit einem Farbfilter versehen).
     */
    cv::Mat detectLines(cv::Mat& filteredImage);

};
}

#endif
