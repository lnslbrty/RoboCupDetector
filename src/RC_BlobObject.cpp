#include "RC_BlobObject.hpp"


rc::BlobObject::BlobObject()
{
  //set values for default constructor
  setType("Object");
  setColor(cv::Scalar(0,0,0));
}

rc::BlobObject::BlobObject(std::string name)
{
  setType(name);
  if(name=="blue") {
    //TODO: use "calibration mode" to find HSV min
    //and HSV max values
    setHSVmin(cv::Scalar(92,0,0));
    setHSVmax(cv::Scalar(124,256,256));
    //BGR value for Green:
    setColor(cv::Scalar(255,0,0));
  }
  if(name=="green") {
    //TODO: use "calibration mode" to find HSV min
    //and HSV max values
    setHSVmin(cv::Scalar(34,50,50));
    setHSVmax(cv::Scalar(80,220,200));
    //BGR value for Yellow:
    setColor(cv::Scalar(0,255,0));
  }
  if(name=="yellow") {
    //TODO: use "calibration mode" to find HSV min
    //and HSV max values
    setHSVmin(cv::Scalar(20,124,123));
    setHSVmax(cv::Scalar(30,256,256));
    //BGR value for Red:
    setColor(cv::Scalar(0,255,255));
  }
  if(name=="red"){
    //TODO: use "calibration mode" to find HSV min
    //and HSV max values
    setHSVmin(cv::Scalar(0,200,0));
    setHSVmax(cv::Scalar(19,255,255));
    //BGR value for Red:
    setColor(cv::Scalar(0,0,255));
  }
}

rc::BlobObject::~BlobObject(void)
{
}

int rc::BlobObject::getXPos()
{
  return this->xPos;
}

void rc::BlobObject::setXPos(int x)
{
  this->xPos = x;
}

int rc::BlobObject::getYPos()
{
  return this->yPos;
}

void rc::BlobObject::setYPos(int y)
{
  this->yPos = y;
}

cv::Scalar rc::BlobObject::getHSVmin()
{
  return this->HSVmin;
}

cv::Scalar rc::BlobObject::getHSVmax()
{
  return this->HSVmax;
}

void rc::BlobObject::setHSVmin(cv::Scalar min)
{
  this->HSVmin = min;
}


void rc::BlobObject::setHSVmax(cv::Scalar max)
{
  this->HSVmax = max;
}
