/**
 * @file    RC_Daemon.hpp
 * @author  Toni Uhlig <matzeton@googlemail.com>
 * @date    16.07.2016
 * @version 1.0
 */

#ifndef RC_DAEMON_H
#define RC_DAEMON_H 1

#include <string>


namespace rc {

/**
 * @name Prüfung ob Dienst bereits terminiert wird/wurde
 * @retval true, wenn Dienst bereits terminiert, sonst false
 */
bool isDaemonTerminate(void);

/**
 * @name Anwendungsstart abschließen
 * @brief Diese Routine sollte nach der Initialisierung aufgerufen werden.
 */
void startUpDone(void);

/**
 * @name Reaktion auf Systemspez. Signale festlegen
 */
void setSignals(void);

/** Klasse zum starten der Anwendung als Systemdienst */
class Daemon {

  public:
    /**
     * @name Instanz der Singleton- Klasse holen
     * @retval Daemon- Objekt
     */
    static Daemon * instance(void) {
      if (!_instance)
        _instance = new Daemon();
      return _instance;
    }
    /**
     * @name Destruktor
     */
    ~Daemon(void) {
    }

    /**
     * @name RoboCup als Systemdienst starten
     * @param Dateipfad zur pidfile
     * @param Dateipfad zur lockfile
     * @param Name des non-root users unter welchem der Dienst laufen soll (DEAKTIVIERT!)
     * @retval true, wenn der Dienst (fork(...)) erfolgreich war, sonst false
     */
    bool Daemonize(std::string pidfile, std::string lockfile, std::string user);

    /**
     * @name RoboCup Systemdienst beenden
     * @param Dateipfad zur pidfile
     * @param Dateipfad zur lockfile
     * @retval true, wenn der Dienst erfolgreich beendet werden konnte, sonst false
     */
    static bool KillByPidfile(std::string pidfile, std::string lockfile);

  private:
    int fLock;                /** Dateideskriptor für die lockfile */
    static Daemon * _instance; /** Zeiger zum Singleton */

    /**
     * @name Konstruktor (privat, da Singleton Klasse)
     */
    explicit Daemon(void) {
    }

};
}

#endif
