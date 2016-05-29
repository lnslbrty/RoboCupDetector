#ifndef RC_BLOBOBJECT_H
#define RC_BLOBOBJECT_H 1

#include <string>
#include <raspicam/raspicam_cv.h>


namespace rc {
class BlobObject
{
    public:
      BlobObject();
      ~BlobObject(void);

      BlobObject(std::string name);

      int getXPos();
      void setXPos(int x);

      int getYPos();
      void setYPos(int y);

      cv::Scalar getHSVmin();
      cv::Scalar getHSVmax();

      void setHSVmin(cv::Scalar min);
      void setHSVmax(cv::Scalar max);

      std::string getType() { return type; }
      void setType(std::string t) { type = t; }

      cv::Scalar getColor(){
        return Color;
      }
      void setColor(cv::Scalar c){
        Color = c;
      }

    private:
      int xPos, yPos;
      std::string type;
      cv::Scalar HSVmin, HSVmax;
      cv::Scalar Color;
};
}

#endif
