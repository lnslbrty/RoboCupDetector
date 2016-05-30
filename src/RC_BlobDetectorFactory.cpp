#include "RC_BlobDetectorFactory.hpp"

std::mutex rc::BlobDetectorFactory::cBuf_mtx;

bool rc::BlobDetectorFactory::doSmth = true;
unsigned int rc::BlobDetectorFactory::numThreads = 1;
std::thread * rc::BlobDetectorFactory::thrds = nullptr;

rc::Camera * rc::BlobDetectorFactory::cam = nullptr;
rc::CircularBuffer<cv::Mat> * rc::BlobDetectorFactory::cBuf = nullptr;
