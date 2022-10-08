#pragma once

//  Подключаем стандартную библиотеку для работы с Shield'ом по шине SPI
#include "SPI.h"
//  Подключаем стандартную библиотеку для работы с Ethernet
#include "Ethernet.h"

#define U_max 280
#define U_min 20


class EthernetSupport {
  public:
    EthernetSupport(EthernetServer& server) : server_(server) {
      
      }

    void ServerBegin(byte mac[]) {
        //  Выводим в монитор последовательного порта сообщение о начале инициализации Shield'а
      Serial.println("Initialize Ethernet with DHCP:");
      //  Если соединение с динамической адресацией не было установлено, то
      if (Ethernet.begin(mac) == 0) {
        //  Выводим сообщение об этом в монитор последовательного порта и
        Serial.println("Failed to configure Ethernet using DHCP");
        //  проверяем наличие самого Shield'а
        if (Ethernet.hardwareStatus() == EthernetNoHardware) {
          //  Если Shield не найден, то выводим соответствующее сообщение в монитор порта
          Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :( ");
          //  Ничего больше не выполняется
          while (true) {
            delay(1); // do nothing, no point running without Ethernet hardware
          }
        }
        //  Проверяем наличие соединения
        if (Ethernet.linkStatus() == LinkOFF) {
          //  Если соеднинение не установлено, то выводим соответствующее сообщение в монитор порта
          Serial.println("Ethernet cable is not connected.");
        }
        // Пробуем установить соединение, используя статический IP-адрес
      //    Ethernet.begin(mac, ip, myDns);
      }
      //  Если соединение с динамической адресацией было установлено, то
      else {
        //  Выводим в монитор порта соответствующее сообщение об этом и выводим назначенный устройству IP-адрес
        Serial.print("  DHCP assigned IP ");
        Serial.println(Ethernet.localIP());
      }
      
      server_.begin(); // начинаем работу сервера
  }

    int EthernetParser(int U_set, double U_current) {
    EthernetClient client = server_.available();
    // проверяем если кто то пытается получить доступ к назначенному IP адресу из браузера
    if (client) {
      //if connected, continue checking if client is sending some message back to the server
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();             
                //read char by char HTTP request (считываем символ за символом)
                if (readString.length() < 100) {
                    //store characters to string (сохраняем считываемые символы в строку)
                    readString += c;
                }
                //if HTTP request has ended– 0x0D is Carriage Return \n ASCII (если передача завершена)
                if (c == 0x0D) {
                  //display the webpage (отображаем веб-страницу)
                    client.println("HTTP/1.1 200 OK"); //send new page
                    client.println("Content-Type: text/html");
                    client.println();
                    client.println("<HTML>");
                    client.println("<HEAD>");
                    client.println("<TITLE> ARDUINO ETHERNET</TITLE>");
                    client.println("</HEAD>");
                    client.println("<BODY>");
                    client.println("<hr>");
                    client.println("<H1 style=\"color:green;text-align:center\">ARDUINO ETHERNET AC CONTROL</H1>");
                    client.println("<hr>");
                    client.println("<br>");

                    client.println("<p style=\"text-align: center\">");
                    client.print("On this page you can request the current voltage ");
                    client.println("</p>");
                    client.println("<H2 style=\"text-align:center\"><a href=\"/?get_U\"\">Get voltage</a><br></H2>");
                    client.println("<br>");
                    
                    client.println("<p style=\"text-align: center\">");
                    client.print("To set 150 V enter in the search bar or use curl http://");
                    client.print(Ethernet.localIP());
                    client.print("/?set_U150");
                    client.println("</p>");
                    
                    client.println("<br>");
                    client.println("</BODY>");
                    client.println("</HTML>");
                    
                    
                    if(readString.indexOf("?set_U") > -1) {   
                      int data = GetVotageFromStr();
                      U_set = constrain(data, U_min, U_max);
                      client.println("<p style=\"text-align: center\">");
                      client.print("Voltage to set: ");
                      client.println(U_set);
                      client.println("</p>");
                      }
                    if(readString.indexOf("?get_U") > -1) {  
                      client.println("<p style=\"text-align: center\">"); 
                      client.print("Current voltage: ");
                      client.println(U_current);
                      client.println("</p>");
                      }

                    delay(10);
                    //stopping client (останавливаем клиент)
                    client.stop();
                    //очищаем строку для последующего использования
                    readString="";
                }
            }
        }
    }
    return U_set;
  }

  private: 
  
      String readString;
      EthernetServer& server_;
    
      int GetVotageFromStr() {
      String tmp_str;
      for (int i = readString.indexOf("?set_U") + 6; i < 15; i++) {
        tmp_str += readString[i];
        if (isSpace(readString[i])) {
          break;
          }
        }
        return tmp_str.toInt();
      }
  
  };
