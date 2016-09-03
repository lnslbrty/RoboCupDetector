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
    WebServer(bool listen_localhost, uint16_t port, size_t image_count, size_t js_refreshrate) {
      this->listen_localhost = listen_localhost;
      this->port = port;
      this->image_cnt = image_count;
      this->imencode_flags.push_back(0);
      this->frames = 0;
      this->js_refreshrate = js_refreshrate;
    }

    /**
     * @name Destruktor
     */
    ~WebServer() {
    }

    bool start();
    void stop(void);

    void setImage(int idx, cv::Mat& src) {
      images_mtx.lock();
      images_out[idx % image_cnt] = src.clone();
      frames++;
      images_mtx.unlock();
    }
    cv::Mat getImage(int idx) {
      images_mtx.lock();
      cv::Mat ret = images_out[idx % image_cnt];
      images_mtx.unlock();
      return ret;
    }
    size_t getFrames(void) {
      images_mtx.lock();
      auto ret = frames;
      images_mtx.unlock();
      return ret;
    }
    size_t getCount(void) { return this->image_cnt; }
    std::vector<int>& getFlags(void) { return this->imencode_flags; }
    size_t getJSRefreshRate(void) { return this->js_refreshrate; }

  private:
    std::mutex images_mtx;
    cv::Mat * images_out;
    std::vector<int> imencode_flags;
    size_t image_cnt, frames, js_refreshrate;
    bool listen_localhost;
    uint16_t port;
    struct MHD_Daemon * httpd = NULL;

};
}

#endif
