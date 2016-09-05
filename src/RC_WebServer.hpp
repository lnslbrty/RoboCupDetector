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
/** Webserver Klasse, um dem Nutzer eine möglichst einfache Rückmeldung zu geben */
class WebServer {

  public:
    /**
     * @name Standartkonstruktor
     * @param true, wenn nur auf localhost eingehende Verbindungen akzeptiert werden sollen, andernfalls auf allen Interfaces zulassen
     * @param 16-Bit TCP Port
     * @param Anzahl anzuzeigender Bilder auf der Website
     * @þaram Zeitdauer, welche das automatische nachladen der Bilder auf der Website setzt (in ms)
     */
    WebServer(bool listenLocalhost, uint16_t port, size_t imageCount, size_t jsRefreshRate) {
      this->listenLocalhost = listenLocalhost;
      this->port = port;
      this->imageCount = imageCount;
      this->imEncodeFlags.push_back(0);
      this->frames = 0;
      this->jsRefreshRate = jsRefreshRate;
    }
    /**
     * @name Destruktor
     */
    ~WebServer() {
    }

    /**
     * @name WebServer starten
     * @brief Der WebServer soll nun Verbindungen entgegen nehmen und beantworten.
     * @retval true, wenn der Start erfolgreich, andernfalls false
     */
    bool start();

    /**
     * @name WebServer stoppen
     * @brief Keine neuen Verbindungen entgegen nehmen und bereits bestehende beenden.
     */
    void stop(void);

    /**
     * @name ein Bild zum abrufen freigeben
     * @param Bildindex
     * @param Bild
     */
    void setImage(int idx, cv::Mat& src) {
      imagesMtx.lock();
      imagesOut[idx % imageCount] = src.clone();
      frames++;
      imagesMtx.unlock();
    }

    /**
     * @name ein Bild holen
     * @param Bildindex
     * @retval Bild
     */
    cv::Mat getImage(int idx) {
      imagesMtx.lock();
      cv::Mat ret = imagesOut[idx % imageCount];
      imagesMtx.unlock();
      return ret;
    }

    /**
     * @name Anzahl Bilder zurückgeben
     * @retval numerischer Wert
     */
    size_t getFrames(void) {
      imagesMtx.lock();
      auto ret = frames;
      imagesMtx.unlock();
      return ret;
    }

    size_t getCount(void) { return this->imageCount; }
    std::vector<int>& getFlags(void) { return this->imEncodeFlags; }
    size_t getJSRefreshRate(void) { return this->jsRefreshRate; }

  private:
    std::mutex imagesMtx;
    cv::Mat * imagesOut;
    std::vector<int> imEncodeFlags;
    size_t imageCount, frames, jsRefreshRate;
    bool listenLocalhost;
    uint16_t port;
    struct MHD_Daemon * httpd = NULL;

};
}

#endif
