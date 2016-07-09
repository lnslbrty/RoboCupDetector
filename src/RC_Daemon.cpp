#include "RC_Daemon.hpp"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <atomic>
#include <chrono>
#include <thread>

#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>


rc::Daemon* rc::Daemon::_instance = nullptr;
static std::atomic_bool gotSigTerm(false);


bool rc::isDaemonTerminate(void) {
  return gotSigTerm;
}

static void signal_handler(int sig) {
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
  sigset_t newSigSet;
  int i;

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
    perror("Daemon::Daemonize open");
    goto error;
  }

  /* der Prozess kann die Datei sperren, sofern sie noch nicht von einem anderen Prozess gesperrt wurde */
  if ( lockf(fLock, F_TLOCK, 0) != 0 ) {
    perror("Daemon::Daemonize ftrylockfile");
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

  chdir("/");

  gotSigTerm = false;
  return true;
error:
  std::cerr <<"Daemon::Daemonize FAILED!"<<std::endl;
  close(fLock);
  return false;
}

int rc::Daemon::KillByPidfile(std::string pidfile) {
  int ret = 1;
  std::ifstream pStrm(pidfile);

  if (pStrm.is_open()) {
    std::string sPid;
    if (getline(pStrm, sPid)) {
      signal(SIGQUIT, SIG_IGN);
      pid_t pid = std::stoi(sPid, nullptr, 10);
      if (kill(pid, 0) != 0) {
        std::cerr <<"Process with pid "<<pid<<" not found"<<std::endl;
        return ret;
      }
      std::cout <<"Sending SIGTERM to "<<pid<<std::endl;
      ret = kill(pid, SIGTERM);
      std::cout <<"Waiting for process .."<<std::endl;
      while (kill(pid, 0) == 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
      return ret;
    }
    pStrm.close();
  }
  return ret;
}

