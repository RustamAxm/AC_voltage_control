#pragma once

//  Подключаем стандартную библиотеку для работы с Shield'ом по шине SPI
#include "SPI.h"
//  Подключаем стандартную библиотеку для работы с Ethernet
#include "Ethernet.h"

#define U_max 280
#define U_min 20


class EthernetSupport {
  public:
    EthernetSupport(EthernetServer& server);

    void ServerBegin(byte mac[]);

    int EthernetParser(int U_set, double U_current);

  private: 
    String readString;
    EthernetServer& server_;
  
    int GetVotageFromStr();
    void PageRender(EthernetClient& client);
  };
