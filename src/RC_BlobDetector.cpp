#include "RC_BlobDetector.hpp"

cv::Mat rc::BlobDetector::process(cv::Mat& image) {
  /* Filter für rote Objekte */
  /*
  cv::Mat mask1 = this->filterByColorRange(image, cv::Scalar(0, 0, 0), cv::Scalar(70, 70, 255));
  cv::Mat mask2 = this->filterByColorRange(image, cv::Scalar(0, 0, 0), cv::Scalar(90, 90, 90));
  cv::Mat mask3 = this->filterByColorRange(image, cv::Scalar(220, 220, 220), cv::Scalar(255, 255, 255));
  */
  /* Filter für gelbe Objekte */
  cv::Mat mask1 = this->filterByColorRange(image, cv::Scalar(0, 120, 120), cv::Scalar(100, 255, 255));
  return mask1;
}

cv::Mat rc::BlobDetector::detectLines(cv::Mat& filteredImage) {
  std::vector<cv::Vec4i> lines;
  cv::HoughLinesP(filteredImage, lines, 1, CV_PI/180, 50, 50, 10);
  cv::Mat out(filteredImage);

  for (size_t i = 0; i < lines.size(); i++) {
    cv::Vec4i l = lines[i];
    cv::Point p1, p2;
    p1 = cv::Point(l[0], l[1]);
    p2 = cv::Point(l[2], l[3]);
    //calculate angle in radian,  if you need it in degrees just do angle * 180 / PI
    //float angle = atan2(p1.y - p2.y, p1.x - p2.x);
    //std::cout << "Angle: "<<angle<<std::endl;
#ifdef USE_XWINDOW
    cvtColor(filteredImage, out, CV_GRAY2BGR);
    cv::line(out, p1, p2, cv::Scalar(255, 255, 0), 5);
#endif
  }
  return out;
}
