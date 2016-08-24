#include "RC_BlobDetector.hpp"


cv::Mat rc::BlobDetector::process(cv::Mat& image, enum roboColor rc) {
  cv::Mat imgHSV, imgThresh;

  /* Farbe filtern */
  imgThresh = this->filterColor(image, rc);
  /* Bild unscharf gestalten (Bildfehler/Farbrauschen korrigieren) */
  cv::medianBlur(imgThresh, imgThresh, 13);

  /* Konturen suchen und im Vector `contours` speichern */
  std::vector<std::vector<cv::Point>> contours;
  cv::Mat imgContour = imgThresh.clone();
  cv::findContours(imgContour, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

  /* Konturen im original Bild nachzeichnen, dabei ein Rechteck als Form der Kontur annehmen */
  cv::Scalar colors[3];
  colors[0] = cv::Scalar(255, 0, 0);
  colors[1] = cv::Scalar(255, 0, 0);
  colors[2] = cv::Scalar(255, 0, 0);
  for (size_t idx = 0; idx < contours.size(); idx++) {
    /* Konturen nachzeichnen */
    cv::drawContours(image, contours, idx, colors[idx % 3]);
    /* minimales Rechteck in die Kontur einpassen */
    cv::RotatedRect rotRect = cv::minAreaRect(contours[idx]);
    cv::Point2f rect_points[4]; 
    rotRect.points(rect_points);
    float angle = rotRect.angle;
    cv::Point2f center = rotRect.center;
    for (size_t jdx = 0; jdx < 4; ++jdx)
      cv::line(image, rect_points[jdx], rect_points[(jdx+1)%4], cv::Scalar(0,255,0));
    std::stringstream ss; ss << angle;
    cv::circle(image, center, 5, cv::Scalar(0,255,0));
    cv::putText(image, ss.str(), center + cv::Point2f(-25,25), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(255,0,255));
  }

  return imgThresh;
}
