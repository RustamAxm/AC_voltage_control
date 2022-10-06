
#include "ZMPT101B.h"
#include <GyverStepper.h>
//GStepper<STEPPER4WIRE> stepper(2048, 5, 3, 4, 2);

//  Подключаем стандартную библиотеку для работы с Shield'ом по шине SPI
#include "SPI.h"
//  Подключаем стандартную библиотеку для работы с Ethernet
#include "Ethernet.h"

#define step_ 2
#define dir_ 5
#define EPSILON 5
#define U_max 280
#define U_min 20

GStepper<STEPPER2WIRE> stepper(3200, step_, dir_);
ZMPT101B voltageSensor(A3, 327.579, 1.34);

double U_set = 0.0;
double U_current = 0.0;
long int value = 0;
static uint32_t tmr2;
static uint32_t tmr1;
int inInt = 10;

String readString; // в этой строке мы будем сохранять сообщение от клиента (пользователя)

EthernetServer server(80);
// Задаём MAC-адрес устройству. Главное, чтобы в сети не было уже зарегистрированно устройство с таким адресом
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

bool IsValidVoltage(float voltage) {
  return voltage > 10 && voltage < 280;
  }

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
      
    Serial.print("val = ");
    Serial.println(value);
   
    Serial.print(stepper.getTarget());
    Serial.print(',');
    Serial.println(stepper.getCurrent());
    Serial.print("U = ");
    Serial.println( U_current);
    Serial.print("U_set = ");
    Serial.println( U_set);

    Serial.print("Serial received: ");
    Serial.println(inInt);
    }
}

void SerialParser() {
  if (Serial.available()) {
    if (Serial.find('w')) {
      inInt = Serial.parseInt();
      U_set = constrain(inInt, U_min, U_max);
      } 
    }
  }

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

void setup() {
  Serial.begin(115200);
  
  stepper.setRunMode(FOLLOW_POS);   // режим следования к целевй позиции
  stepper.setMaxSpeed(200);         // установка макс. скорости в шагах/сек
  stepper.setAcceleration(200);     // установка ускорения в шагах/сек/сек

  // пусть драйвер выключается при достижении позиции
  stepper.autoPower(true);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A3, INPUT);
  
  U_set = voltageSensor.getVoltageAC_custom();
  Serial.println("Done!");

  delay(100);
  ServerBegin();
}


void loop() {
  U_current = voltageSensor.getVoltageAC_custom();
  
  stepper.tick();

  if (millis() - tmr1 > 50) {
    tmr1 = millis();
    
    if (digitalRead(A0) == 1 && U_set < U_max) {
      U_set += 2;
      }
        
    if (digitalRead(A1) == 1 && U_set > U_min) {
      U_set -= 2;
      }

    if (abs(U_current - U_set) > EPSILON && U_current < U_set) {
      value -= 10;
      }

    if (abs(U_current - U_set) > EPSILON && U_current > U_set) {
      value += 10;
      }
      GoToTarget(value);
  }


  SerialParser();
  EthernetParser(); 
 
  PrintData(); 
}
