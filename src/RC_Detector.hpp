/**
 * @file    RC_Detector.hpp
 * @author  Toni Uhlig <matzeton@googlemail.com>
 * @date    25.05.2016
 * @version 1.0
 */

#ifndef RC_BLOBDETECTOR_H
#define RC_BLOBDETECTOR_H 1

#include <opencv2/opencv.hpp>


namespace rc {

/** zu filternde Farbe */
enum roboColor {
  RB_YELLOW, RB_BLUE
};


/* Makros für die Geschwindigkeitsmessung verschiedener Bearbeitungsschritte */
/** vergangene "Zeit" (Ticks) messen */
#define measureTicks(var) { var = cv::getTickCount(); }
/** "Zeit"-Differenz berechnen (Durchschnitt von `end-start` und `dest`) */
#define calcDiffTime(start, end, dest) { float tmp = dest; dest = (end - start) / cv::getTickFrequency(); dest = (dest + tmp)/2; }

/** Datenstruktur für Zeitaufwand der Teilschritte */
struct time_consumption {
  /** Zeitaufwand für Farbfilter */
  float avg_color    = 0;
#ifdef ENABLE_BLUR
  /** Zeitaufwand für Unschärfefilter */
  float avg_blur     = 0;
#endif
  /** Zeitaufwand für Konturenfilter */
  float avg_contours = 0;
  /** Zeitaufwand für das Nachzeichnen im Originalbild */
  float avg_draw     = 0;
};

/** Durchschnittswerte für Zeitaufwand holen */
struct time_consumption & getAvgTime(void);


/** wichtige Informationen eines bearbeiteten Bildes in dieser Struktur speichern */
struct processed_image {
  /** X-Koordinate (X=0 entspricht Breite des Bildes geteilt durch 2) */
  float midx;
  /** Y-Koordinate (Y-Koordinate entspricht Y-Koordinate im Urpsrungsbild) */
  float absy;
  /** Winkel des rotierenden Rechteckes */
  float angle;
  /** die (geschätzte!) Entfernung zur Kamera */
  float distance;
};

/** Detektor- Klasse zum filtern/auswerten von Bildern */
class Detector {

  public:
    /**
     * @name Konstruktor
     */
    Detector() {
    }

    /**
     * @name Destruktor
     */
    ~Detector() {
    }

    /**
     * @name Farbfilter
     * @param Quellbild
     * @param zu filternde Farbe
     * @retval die gefilterte OpenCV Matrix
     */
    cv::Mat filterColor(cv::Mat src, enum roboColor rcol) {
      cv::Mat tmp, result;
      // konvertieren von RGB zu HSV
      cv::cvtColor(src, tmp, CV_BGR2HSV);
      // filtere HSV- Farbraum entsprechend der angegebenen roboColor
      switch (rcol) {
        case RB_YELLOW: /* GELB */
          cv::inRange(tmp, cv::Scalar(20, 100, 100), cv::Scalar(40, 255, 255), result);
          break;
        case RB_BLUE:   /* BLAU */
          cv::inRange(tmp, cv::Scalar(100, 100, 100), cv::Scalar(120, 255, 255), result);
          break;
      }
      return result;
    }

    /**
     * @name Sucht nach Konturen in einem Bild
     * @param Das zu filternde Bild.
     * @param die zu filternde Farbe
     * @param ausgewertete Informationen in dieser Struktur speichern
     * @retval Matrix nach dem Farbfilter
     */
    cv::Mat process(cv::Mat& image, enum rc::roboColor rc, rc::processed_image& pi, rc::time_consumption& tc);

};
}

#endif
