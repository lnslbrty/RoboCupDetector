#include "RC_Threads.hpp"


rc::Threads::Threads(unsigned int numThreads) {
  this->doLoop = false;
  this->numThreads = numThreads;
  /* Thread-Objekte instanziieren */
  thrds = new std::thread[numThreads];
}

rc::Threads::~Threads(void) {
  /* Destruktoren aufrufen und Speicher freigeben */
  delete[] thrds;
}

void rc::Threads::startThreads(void) {
  if (thrdFunc == nullptr)
    return;
  /* alle Threads erstellen + starten */
  for (unsigned int i = 0; i < numThreads; ++i) {
    /* Thread Einsprungsfunktion über Lambda zuweisen */
    thrds[i] = std::thread([this](unsigned int num, threadFunc_t func) {
      /* benutzerdefinierten Threadnamen zuweisen */
      auto handle = thrds[num].native_handle();
      std::string pname(thrdPrefix);
      pname += " worker";
      pname += std::to_string(num);
      pthread_setname_np(handle, pname.c_str());
      /* Initialisierungsfunktion aufrufen, falls angegeben */
      if (initFunc != nullptr)
        initFunc(num);
      doLoop = true;
      /* Hauptschleife der Arbeiter- Threads */
      while (doLoop) {
        /* Thread-"Rückruf"-Funktion */
        func(num);
      }
      /* Aufräumfunktion aufrufen, falls angegeben */
      if (cleanupFunc != nullptr)
        cleanupFunc(num);
    }, i, thrdFunc);
  }
}

void rc::Threads::stopThreads(void) {
  doLoop = false;
  for (unsigned int i = 0; i < numThreads; ++i) {
    /* auf das Ende aller Arbeiter- Threads warten */
    thrds[i].join();
  }
}
