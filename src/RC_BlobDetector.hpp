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
     cv::Mat process(cv::Mat& image);

};
}

#endif
