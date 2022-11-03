
#define step_ 2
#define dir_ 5
#define en_ A6
#define EPSILON 2
#define U_max 280
#define U_min 20
#define Manual_Step 5

#include <GyverStepper.h>
#include <GyverOLED.h>
#include "EthernetSupport.h"
#include "ZMPT101B.h"

GStepper<STEPPER2WIRE> stepper(3200, step_, dir_, en_);
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;
ZMPT101B voltageSensor(A3, 327.579, 1.34);

EthernetServer server(80);
EthernetSupport sup(server);


// Задаём MAC-адрес устройству. Главное, чтобы в сети не было уже зарегистрированно устройство с таким адресом
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
double U_set = 0.0;
double U_current = 0.0;
long int value = 0;

static uint32_t tmr1;
static uint32_t tmr2;
static uint32_t tmr3;

#include "support_functions.h"


void setup() {
  Serial.begin(115200);
  
  stepper.setRunMode(FOLLOW_POS);  
  stepper.setMaxSpeed(100);         // установка макс. скорости в шагах/сек
  stepper.setAcceleration(200);     // установка ускорения в шагах/сек/сек
  stepper.autoPower(true);
  stepper.enable();
  
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A3, INPUT);
  pinMode(A6, OUTPUT);
  
  U_set = constrain(voltageSensor.getVoltageAC_custom(), U_min, U_max);
  Serial.println("Done!");

  delay(100);
  sup.ServerBegin(mac);

  oled.init();        // инициализация
  oled.clear();
  oled.setContrast(32);
  delay(100);
}

void loop() {
  //refresh dhcp
  if (millis() - tmr3 > 1000 * 36000) {
    tmr3 = millis();
    sup.ServerBegin(mac);
  }

  stepper.tick();
  
  PrintIPOled();
  PrintSetOled();
  U_current = voltageSensor.getVoltageAC_custom();
  
  DiscretStep();
 
  SerialParser();
  U_set = sup.EthernetParser(U_set, U_current); 
 
  PrintData(); 
}
