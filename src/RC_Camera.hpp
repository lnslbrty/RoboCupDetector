/**
 * @file   RC_Camera.hpp
 * @date   25.05.2016
 * @author Toni Uhlig <matzeton@googlemail.com>
 * @brief  Eine Klasse zur Steuerung der Kamera und des Zwischenspeichers (Puffer)
 *
 * Diese Klasse ermöglicht eine einfache Steuerung der Raspicam (mit OpenCV
 * Unterstützung). Dabei erbt rc::Camera von beiden Klassen `protected`.
 * Für mehr Informationen: https://de.wikibooks.org/wiki/C%2B%2B-Programmierung:_Vererbung
 */

#ifndef RC_CAMERA_H
#define RC_CAMERA_H 1

#include <raspicam/raspicam.h>
#include <raspicam/raspicam_cv.h>
#include <raspicam/raspicamtypes.h>

#include "RC_CircularBuffer.hpp"


namespace rc {
class Camera : protected raspicam::RaspiCam_Cv, protected rc::CircularBuffer<cv::Mat> {

  public:
    /**
     * @name   Kamera Konstruktor
     * @param  [in] maxBufferElements größe des Pufferspeichers (@ref rc::CircularBuffer)
     * @retval Kamera Objekt
     */
    Camera(unsigned int maxBufferElements);
    /**
     * @name  Standart Destruktor
     * @brief Dieser ist z.Z. ausreichend, da keine "aufräum"- Arbeiten nötig sind.
     */
    ~Camera(void) {}

    /**
     * @name   Kamerabild holen
     * @brief  Ein Kamerabild "grabschen".
     * @retval TRUE  Kamerabild konnte erfolgreich geholt werden
     * @retval FALSE entweder lieferte die Kamera kein bild oder der Pufferspeicher ist voll
     *
     * Wichtig: Damit ein Bild von der Kamera geholt werden kann, muss vorher
     * raspicam::RaspiCam_Cv::open() aufgerugen werden!
     */
    bool grabCameraImage(void);

    /**
     * @name   Breite eines Kamerabildes zurückgeben
     * @retval die Breite in Pixel
     */
    unsigned int getWidth(void) { return this->get(CV_CAP_PROP_FRAME_WIDTH); }

    /**
     * @name   Höhe eines Kamerabildes zurückgeben
     * @retval die Höhe in Pixel
     */
    unsigned int getHeight(void) { return this->get(CV_CAP_PROP_FRAME_HEIGHT); }

    /**
     * @name   Breite eines Kamerabildes festlegen
     * @param die Breite in Pixel
     */
    void setWidth(unsigned int width) { this->set(CV_CAP_PROP_FRAME_WIDTH, width); }

    /**
     * @name   Höhe eines Kamerabildes festlegen
     * @param die Höhe in Pixel
     */
    void setHeight(unsigned int height) { this->set(CV_CAP_PROP_FRAME_HEIGHT, height); }

    /**
     * @name   Format der Kamerabilder zurückgeben
     * @retval CV_8UC1 Schwarz-Weiß (1 Byte Farbabstufung)
     * @retval CV_8UC3 Farbe (3 Byte pro Kanal -> RGB)
     */
    raspicam::RASPICAM_FORMAT getFormat(void) { return static_cast<raspicam::RASPICAM_FORMAT>(this->get(CV_CAP_PROP_FORMAT)); }

    /**
     * @name   Farbsättigung zurückgeben
     * @retval ein Wert zwischen 0 (keine Sättigung) und 100 (volle Sättigung)
     */
    unsigned int getSaturation(void) { return this->get(CV_CAP_PROP_SATURATION); }

    /**
     * @name   Bildverstärkung zurückgeben (Treiber spezifisch)
     * @retval ein Wert zwischen 0 (keine Verstärkung) und 100 (maximale Verstärkung)
     */
    unsigned int getGain(void) { return this->get(CV_CAP_PROP_GAIN); }

    /**
     * @name   Belichtungsdauer zurückgeben
     * @retval ein Wert zwischen 1 (kurze Belichtungsdauer) und 100 (lange BElichtungsdauer) oder -1 ffür eine automatische Belichtungsdauer
     */
    int getExposure(void) { return this->get(CV_CAP_PROP_EXPOSURE); }

    unsigned int getMaxFPS(void) { return this->get(CV_CAP_PROP_FPS); }

    /**
     * @name   Sättigung festlegen
     * @param  [in] value siehe @ref rc::Camera::getSaturation()
     */
    void setSaturation(unsigned int value) { this->set(CV_CAP_PROP_SATURATION, value); }

    /**
     * @name   Bildverstärkung festlegen
     * @param  [in] value siehe @ref rc::Camera::getGain()
     */
    void setGain(unsigned int value) { this->set(CV_CAP_PROP_GAIN, value); }

    /**
     * @name   Belichtungsdauer festlegen
     * @param  [in] value siehe @ref rc::Camera::getExposure()
     */
    void setExposure(int value) { this->set(CV_CAP_PROP_EXPOSURE, value); }

    void setMaxFPS(unsigned int value) { this->set(CV_CAP_PROP_FPS, value); }

};
}

#endif
