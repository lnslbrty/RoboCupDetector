/**
 * @file    RC_DetectorFactory.hpp
 * @date    25.05.2016
 * @author  Toni Uhlig <matzeton@googlemail.com>
 * @version 1.0
 */

#ifndef RC_BLOBDETECTORFACTORY_H
#define RC_BLOBDETECTORFACTORY_H 1

#include <iostream>
#include <ostream>
#include <raspicam/raspicam_cv.h>

#include "RC_Camera.hpp"
#include "RC_Detector.hpp"
#include "RC_Threads.hpp"
#ifdef USE_XWINDOW
#include "RC_Window.hpp"
#endif
#ifdef ENABLE_HTTPD
#include "RC_WebServer.hpp"
#endif


namespace rc {

/** Enumerator für erzeugte/verarbeitete Bilder */
enum roboImage {
  /** modifiziertes Originalbild */
  IMAGE_DRAWED = 0,
  /** Originalbild mit Gelb- Farbfilter */
  IMAGE_FILTERED_YELLOW,
  /** Originalbild mit Blau- Farbfilter */
  IMAGE_FILTERED_BLUE,
  /** Anzahl der Einträge in diesem Enumerator */
  IMAGE_MAX
};

/** Datenstruktur für Thread- Daten (per Thread jeweils) */
struct threadData {
  /** ausgewertete Bildinformationen (Gelb) */
  rc::processed_image * piY = nullptr;
  /** ausgewertete Bildinformationen (Blau) */
  rc::processed_image * piB = nullptr;
  /** Zeitaufwand für beide obigen Vorgänge */
  rc::time_consumption * tc = nullptr;
};

/** "Fabrik"-Klasse, d.h. hier befindet sich die Hauptlogik */
class DetectorFactory : private rc::Detector, public rc::Camera {

  public:
    /**
     * @name Konstruktor
     * @param [in] Anzahl der zu "Arbeiter"- Threads
     */
    DetectorFactory(unsigned int numThreads);
    /**
     * @name Standart Destruktor
     */
    ~DetectorFactory();

    /**
     * @name Initialisierung der Kamera
     * @brief Hinweis: Diese Methode sollte _vor_ @see startThreads ausgeführt werden
     */
    bool openCamera(void) { return this->open(); }
    /**
     * @name Deaktiviert die Kamera
     * @brief Hinweis: Diese Methode sollte _nach_ @see stopThreads ausgeführt werden
     */
    void closeCamera(void) { return this->release(); }
    /**
     * @name Startet die "Arbeiter"- Threads
     * @brief Die Threads werden gestartet und befinden sich in einer Endlosschleife,
     *        solange das Programm nicht beendet wird oder @see stopThreads() aufgerufen wird.
     */
    void startThreads(void);
    /**
     * @name Stoppt alle "Arbeiter"- Threads
     * @brief Alle Threads werden beendet. Erst nachdem alle Threads beendet wurden, kerht diese
     *        Methode zum aufrufenden zurück.
     */
    void stopThreads(void);
    /**
     * @name Gibt Informationen über diese Klasse als String zurück.
     * @brief Dabei werden Informationen über den CircularBuffer und der Window- Klasse ausgegeben.
     * @return Die Informationen als Zeichenkette.
     */
    std::string outInfo(void);

    /**
     * @name Setzt die Groese des Vorschau- Videos.
     * @param Breite in Pixel
     * @param Hoehe in Pixel
     * @brief Dabei ist zu beachten, dass die moeglichst kleine Werte gewaehlt werden sollten,
              um den Raspberry nicht zu ueberlasten.
     */
    void setDimensions(unsigned int width, unsigned int height) {
      this->width = width;
      this->height = height;
      this->setWidth(width);
      this->setHeight(height);
    }

#ifdef ENABLE_HTTPD
    /**
     * @name Setze WebServer Instanz
     * @param rc::WebServer Instanz
     */
    void setHttpd(rc::WebServer * httpd) {
      this->httpd = httpd;
    }

    /**
     * @name Zeige Bilder mit Farbfilter im Webinterface an
     * @param true bedeutet Bilder anzeigen
     */
    void setShowFiltered(bool enable) { if (!httpd) showFiltered = enable; }
#endif

#ifdef ENABLE_VIDEO
    /**
     * @name Setzt den Dateiname fuer das Vorschau- Video.
     * @param Dateiname als String
     */
    void setVideoOut(char* filename) { this->filename = filename; }
#endif

#ifdef USE_XWINDOW
    /**
     * @name Gibt die aktuelle Instanz des Vorschau- Fensters zurueck (Singleton).
     */
    static rc::Window * getXWindow(void) { return win; }
#endif

  private:
    unsigned int width, height;           /** Groese der Basis Bilder */
    struct threadData * data = nullptr;   /** Zeiger auf Thread spezifische Daten */
    rc::Threads * threads = nullptr;      /** Zeiger auf Threadpool */
#ifdef USE_XWINDOW
    static rc::Window * win;              /** Zeiger auf Vorschau- Fenster */
#endif
#ifdef ENABLE_VIDEO
    char* filename = nullptr;             /** Dateiname fuer Video- Ausgabe */
    cv::VideoWriter * videoOut = nullptr; /** OpenCV Video Konverter/Ausgabe */
    std::mutex videoMtx;                  /** Mutex fuer schreiben eines Ausgabebildes in den Videodatemstrom */
#endif
#ifdef ENABLE_HTTPD
    rc::WebServer * httpd = nullptr;      /** Zeiger auf WebServer Objekt */
    bool showFiltered;                    /** Bilder mit angewendetem Farbfilter anzeigen */
#endif
    std::mutex out;                       /** Ausgabemutex für Threads */
};
}

#endif
