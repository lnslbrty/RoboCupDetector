#include "RC_Daemon.hpp"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <atomic>
#include <chrono>
#include <thread>

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pwd.h>


rc::Daemon* rc::Daemon::_instance = nullptr; /** Singleton Instanz */
static std::atomic_bool gotSigTerm(false);   /** bei Erhalt eines SIGTERMS/SIGINT (Ctrl+c) true, sonst false */
static std::atomic_bool isStartup(false);    /** Anwendungsstart bereits abgeschlossen? */


bool rc::isDaemonTerminate(void) {
  return gotSigTerm;
}

void rc::startUpDone(void) {
  isStartup = true;
}

static void signal_handler(int sig) {
  if (!isStartup)
    return;
  switch(sig) {
    case SIGHUP:
      break;
    case SIGINT:
    case SIGTERM:
      gotSigTerm = true;
      break;
    default:
      break;
  }
}

void rc::setSignals(void) {
  struct sigaction newSigAction;

  /* Eigene Signal- Bearbeitungs Routine initialisieren */
  newSigAction.sa_handler = signal_handler;
  sigemptyset(&newSigAction.sa_mask);
  newSigAction.sa_flags = 0;
  /* zu bearbeitende Signale */
  sigaction(SIGHUP, &newSigAction, NULL);
  sigaction(SIGTERM, &newSigAction, NULL);
  sigaction(SIGINT, &newSigAction, NULL);
}

bool rc::Daemon::Daemonize(std::string pidfile, std::string lockfile, std::string user) {
  /* wird z.Z. nicht benötigt -> siehe libmmal Problem weiter unten */
  //struct passwd * pwd = getpwnam(user.c_str());
  sigset_t newSigSet;
  int i;

  /* nur der Benutzer `root` darf einen Dienst starten (notwendig bzgl. Schreibrechte auf /var/run) */
  if (getuid() != 0) {
    std::cerr <<"Daemon::Daemonize: Only root can do this!"<<std::endl;
    exit(2);
  }

  /* Pruefe, ob bereits eine `parent pid` existiert, d.h. Daemonize wurde bereits erfolgreich aufgerufen */
  if (getppid() == 1) {
    return false;
  }

  /* Setze verschiedene Betriebssystem- Signale blockierend */
  sigemptyset(&newSigSet);
  sigaddset(&newSigSet, SIGCHLD);
  sigaddset(&newSigSet, SIGTSTP);
  sigaddset(&newSigSet, SIGTTOU);
  sigaddset(&newSigSet, SIGTTIN);
  sigprocmask(SIG_BLOCK, &newSigSet, NULL);

  /* der eigentlich Daemon Erstellungsprozess */
  pid_t pid;
  if ( (pid = fork()) < 0 ) {
    perror("Daemon::Daemonize fork");
    goto error;
  }
  if (pid > 0) {
    /* der alte Vaterprozess gelangt in diesen Anweisungsblock und wird nicht mehr benötigt */
    exit(EXIT_SUCCESS);
  }

  /* Oeffne/Erstelle eine Dateisperre */
  if ( (fLock = open(lockfile.c_str(),  O_CREAT | O_TRUNC | O_RDWR | O_SYNC)) < 0 ) {
    std::string err("Daemon::Daemonize open(");
    err.append(lockfile);
    err.append(")");
    perror(err.c_str());
    goto error;
  }

  /* der Prozess kann die Datei sperren, sofern sie noch nicht von einem anderen Prozess gesperrt wurde */
  if ( lockf(fLock, F_TLOCK, 0) != 0 ) {
    std::string err("Daemon::Daemonize lockfile(");
    err.append(lockfile);
    err.append(")");
    perror(err.c_str());
    goto error;
  }

  /* der neue Kindprozess gelangt in diesen Anweisungsblock */
  umask(027); /* Set file permissions 750 */
  int sid;
  /* neue Prozessgruppe erzeugen und in `sid` ablegen */
  if ( (sid = setsid()) < 0 ) {
    exit(EXIT_FAILURE);
  }

  pid = getpid();
  std::cout <<"Daemon::Daemonize process id: "<<pid<<std::endl;
  int pFd;
  if ( (pFd = open(pidfile.c_str(), O_CREAT | O_TRUNC | O_RDWR | O_SYNC)) >= 0 ) {
    std::string pStr(std::to_string(pid));
    write(pFd, pStr.c_str(), pStr.length());
  }

  /* Benutzerkontext ändern (hofffentlich non-root) */
  /* ACHTUNG: Die Raspberry IO Bibliothek `libmmal` verursacht Probleme bei nicht-root Benutzer */
  /* TODO: mmal sollte auch als non-root funktionieren */
/*
  if (pwd == NULL) {
    std::cerr <<"Daemon::Daemonize user: "<<user.c_str()<<" not found!"<<std::endl;
    goto error;
  } else std::cout <<"Daemon::Daemonize user: "<<user.c_str()<<"("<<pwd->pw_uid<<")"<<std::endl;
  if (setregid(pwd->pw_gid, pwd->pw_gid)) {
    perror("setuid");
    goto error;
  }
  if (setreuid(pwd->pw_uid, pwd->pw_uid)) {
    perror("setuid");
    goto error;
  }
*/

  /* alle wohlbekannten Deskriptoren deaktivieren */
  close(0); // STDIN
  close(1); // STDOUT
  close(2); // STDERR

  /* alle wohlbekannten Deskriptoren zu /dev/null umleiten */
  /* STDIN */
  i = open("/dev/null", O_RDWR);
  /* STDOUT */
  dup(i);
  /* STDERR */
  dup(i);

  /* in das Wurzelverzeichnis wechseln */
  chdir("/");

  gotSigTerm = false;
  return true;
error:
  std::cerr <<"Daemon::Daemonize FAILED!"<<std::endl;
  close(fLock);
  exit(1);
}

bool rc::Daemon::KillByPidfile(std::string pidfile, std::string lockfile) {
  bool ret = false;
  std::ifstream pStrm(pidfile);

  if (getuid() != 0) {
    std::cerr <<"Only root is allowed to do this!"<<std::endl;
    return ret;
  }
  /* pidfile öffnen */
  if (pStrm.is_open()) {
    std::string sPid;
    /* Inhalt lesen */
    if (getline(pStrm, sPid)) {
      signal(SIGQUIT, SIG_IGN);
      /* pid: String zu Integer */
      pid_t pid = std::stoi(sPid, nullptr, 10);
      /* Prozess auf vorhandensein prüfen */
      if (kill(pid, 0) != 0) {
        std::cerr <<"Process with pid "<<pid<<" not found"<<std::endl;
        return ret;
      }
      std::cout <<"Sending SIGTERM to "<<pid<<std::endl;
      /* Prozess versuchen mit einem `SIGTERM` zu beenden */
      ret = (kill(pid, SIGTERM) == 0 ? true : false);
      std::cout <<"Waiting for process .."<<std::endl;
      /* Warten bis Prozess beendet */
      while (kill(pid, 0) == 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
      /* sowohl pidfile als auch lockfile löschen */
      if (unlink(pidfile.c_str()) != 0 || unlink(lockfile.c_str()))
        std::cerr <<"Unlink ("<<pidfile.c_str()<<" | "<<lockfile.c_str()<<") returned: "<<strerror(errno)<<std::endl;
      std::cout <<"Killed: "<<pid<<std::endl;
      return ret;
    }
    pStrm.close();
  }
  return ret;
}

