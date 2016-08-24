#include "RC_BlobDetector.hpp"


cv::Mat rc::BlobDetector::process(cv::Mat& image, enum roboColor rc, struct processed_image * pi) {
  cv::Mat imgHSV, imgThresh;

  /* Farbe filtern */
  imgThresh = this->filterColor(image, rc);
  /* Bild unscharf gestalten (Bildfehler/Farbrauschen korrigieren) */
  cv::medianBlur(imgThresh, imgThresh, 13);

  /* Konturen suchen und im Vector `contours` speichern */
  std::vector<std::vector<cv::Point>> contours;
  cv::Mat imgContour = imgThresh.clone();
  cv::findContours(imgContour, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);

  /* nach größter Kontur suchen */
  double largest_area = 0.0;
  int largest_contour_index = -1;
  for (size_t i = 0; i < contours.size(); i++) {
    /* berechne Größe der Fläche (Green's Integral) */
    auto a = contourArea(contours[i], false);
    if (a > largest_area) {
      largest_area = a;
      largest_contour_index = i;
    }
  }

  /* Konturen im original Bild nachzeichnen, dabei ein Rechteck als Form der Kontur annehmen */
  if (largest_contour_index >= 0) {
    auto idx = largest_contour_index;
    /* Konturen nachzeichnen */
    cv::drawContours(image, contours, idx, cv::Scalar(255,0,0));
    /* minimales Rechteck in die Kontur einpassen */
    cv::RotatedRect rotRect = cv::minAreaRect(contours[idx]);
    /* vier Eckpunkte des berechneten Rechteckes speichern */
    cv::Point2f rect_points[4]; 
    rotRect.points(rect_points);
    /* Winkel speichern */
    float angle = rotRect.angle;
    /* Mittelpunkt des Rechteckes */
    cv::Point2f center = rotRect.center;

    /* Konturen nachzeichen, Rechteck zeichnen, Mittelpunkt zeichnen, Text schreiben */
    for (size_t jdx = 0; jdx < 4; ++jdx)
      cv::line(image, rect_points[jdx], rect_points[(jdx+1)%4], cv::Scalar(0,255,0));
    std::stringstream ss; ss << angle << "/" << largest_area;
    cv::circle(image, center, 5, cv::Scalar(0,255,0));
    cv::putText(image, ss.str(), center + cv::Point2f(-25,25), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(255,0,255));

    /* Ausgabedatenstruktur beschreiben für später Verwendung (z.B.: Transport über SPI/I²C Schnittstelle) */
    if (pi) {
      for (size_t idx = 0; idx < 4; ++idx)
        pi->coords[idx] = rect_points[idx];
      pi->angle = angle;
      pi->distance = largest_area;
    }
  }

  /* zum Schluß das Bild mit Farbfilter zurückgeben */
  return imgThresh;
}
