#include "RC_BlobDetectorFactory.hpp"

rc::Camera * rc::BlobDetectorFactory::cam = nullptr;
#ifdef USE_XWINDOW
rc::Window * rc::BlobDetectorFactory::win = nullptr;
#endif
