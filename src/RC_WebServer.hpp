/**
 * @file    RC_WebServer.hpp
 * @author  Toni Uhlig <matzeton@googlemail.com>
 * @date    20.08.2016
 * @version 1.0
 */

#ifndef RC_WEBSERVER_H
#define RC_WEBSERVER_H 1

#include <thread>
#include <mutex>

#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>
#include <microhttpd.h>


namespace rc {
class WebServer {

  public:
    /**
     * @name Standartkonstruktor
     */
    WebServer(int port, int image_count) {
      this->port = port;
      this->image_cnt = image_count;
    }

    /**
     * @name Destruktor
     */
    ~WebServer() {
    }

    bool start();
    void stop(void) {
      MHD_stop_daemon(httpd);
      delete[] images_out;
    }

    void setImage(int idx, cv::Mat src) {
      images_mtx.lock();
      images_out[idx] = src;
      images_mtx.unlock();
    }
    cv::Mat getImage(int idx) {
      images_mtx.lock();
      cv::Mat ret = images_out[idx];
      images_mtx.unlock();
      return ret;
    }
    size_t getCount(void) { return this->image_cnt; }

  private:
    std::mutex images_mtx;
    cv::Mat * images_out;
    size_t image_cnt, port;
    struct MHD_Daemon * httpd = NULL;

};
}

#endif
