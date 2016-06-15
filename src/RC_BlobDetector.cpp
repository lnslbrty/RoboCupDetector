#include "RC_BlobDetector.hpp"

void rc::BlobDetector::process(cv::Mat& image) {
  /* Filter für gelbe Objekte */
  cv::Mat mask(image.size(), image.type(), CV_8UC1);
  cv::inRange(image, cv::Scalar(0, 120, 120), cv::Scalar(100, 255, 255), mask);
}

void rc::BlobDetector::detectLines(cv::Mat& filteredImage) {
/*
  std::vector<cv::Vec4i> lines;
  cv::HoughLines(filteredImage, lines, 1, CV_PI/180, 50, 50, 10);

  for (size_t i = 0; i < lines.size(); i++) {
    cv::Vec4i l = lines[i];
    cv::Point p1, p2;
    p1 = cv::Point(l[0], l[1]);
    p2 = cv::Point(l[2], l[3]);
    //calculate angle in radian,  if you need it in degrees just do angle * 180 / PI
    //float angle = atan2(p1.y - p2.y, p1.x - p2.x);
    //std::cout << "Angle: "<<angle<<std::endl;
#ifdef USE_XWINDOW
    cv::line(tmp, p1, p2, cv::Scalar(255, 255, 0), 5);
#endif
  }
*/
}
