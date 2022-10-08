


void GoToTarget(long int value) {
  if (value != stepper.getTarget()){
    stepper.setTarget(value);
    Serial.print("go to");
    Serial.println(value);
    while(stepper.tick()) {
        
      }
    }
  }
   

void PrintData() {
    if (millis() - tmr2 > 500) {
    tmr2 = millis();
      
    Serial.print("value = ");
    Serial.println(value);
    Serial.print("target ");
    Serial.print(stepper.getTarget());
    Serial.print(", ");
    Serial.print("current ");
    Serial.println(stepper.getCurrent());
    Serial.print("U_current = ");
    Serial.println( U_current);
    Serial.print("U_set = ");
    Serial.println( U_set);
    }
}

void SerialParser() {
  if (Serial.available()) {
    if (Serial.find('w')) {
      int inInt = Serial.parseInt();
      U_set = constrain(inInt, U_min, U_max);
      }
    }
  }

String readString; // в этой строке мы будем сохранять сообщение от клиента (пользователя)

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

void EthernetParser() {
    EthernetClient client = server.available();
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
  }
  
void ServerBegin() {
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
  
  server.begin(); // начинаем работу сервера
  }

  int CountSteps(double rhs, double lhs) {
    return abs(rhs - lhs)*100/30;
    }

  void PrintSetOled() {
    oled.setScale(2);
    oled.setCursor(0, 4);
    oled.print("Set=");
    oled.print(U_set);
    }

  void PrintIPOled() {
    if (Ethernet.linkStatus() == LinkON) {
      oled.setScale(1);   // масштаб текста (1..4)
      oled.home();        // курсор в 0,0
      oled.print("IP:");
      oled.print(Ethernet.localIP());
      }
    else if (Ethernet.linkStatus() == LinkOFF) {
      oled.setScale(1);   // масштаб текста (1..4)
      oled.home();        // курсор в 0,0
      oled.print("IP:");
      oled.print("  Link OFF    ");
      }
    }

  void DiscretStep() {
    if (millis() - tmr1 > 50) {
    tmr1 = millis();
    
    if (digitalRead(A0) == 0 && U_set < U_max) {
      U_set += Manual_Step;
      }
    else if (digitalRead(A1) == 0 && U_set > U_min) {
      U_set -= Manual_Step;
      }
    else {
      if (abs(U_current - U_set) > EPSILON && U_current < U_set) {
      value -= CountSteps(U_current, U_set);
      }

      if (abs(U_current - U_set) > EPSILON && U_current > U_set) {
        value += CountSteps(U_current, U_set);
        }
        GoToTarget(value);
      }
      
    }
    
  }
