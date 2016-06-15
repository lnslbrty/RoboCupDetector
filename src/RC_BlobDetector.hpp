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
     * Filtert bestimmte Farbbereiche eines Bildes zur weiterverarbeitung.
     *
     * @param Das zu filternde Bild.
     */
    void process(cv::Mat& image);

    /**
     * Sucht nach Linien in einem Bild.
     *
     * @param Ein Bild (möglichst mit einem Farbfilter versehen).
     */
    void detectLines(cv::Mat& filteredImage);

};
}

#endif
