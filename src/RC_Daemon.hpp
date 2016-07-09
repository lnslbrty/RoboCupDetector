#ifndef RC_DAEMON_H
#define RC_DAEMON_H 1

#include <string>


namespace rc {
bool isDaemonTerminate(void);
void setSignals(void);

class Daemon {

  public:
    static Daemon *instance(void) {
      if (!_instance)
        _instance = new Daemon();
      return _instance;
    }
    ~Daemon(void) {
    }

    bool Daemonize(std::string pidfile, std::string lockfile, std::string user);

    static int KillByPidfile(std::string pidfile);

  private:
    int fLock;

    static Daemon* _instance;

    explicit Daemon(void) {
    }

};
}

#endif
