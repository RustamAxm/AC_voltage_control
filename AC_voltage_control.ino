
#define step_ 2
#define dir_ 5
#define EPSILON 2
#define U_max 280
#define U_min 20
#define Manual_Step 5

#include "ZMPT101B.h"
#include <GyverStepper.h>
//GStepper<STEPPER4WIRE> stepper(2048, 5, 3, 4, 2);

//  Подключаем стандартную библиотеку для работы с Shield'ом по шине SPI
#include "SPI.h"
//  Подключаем стандартную библиотеку для работы с Ethernet
#include "Ethernet.h"

GStepper<STEPPER2WIRE> stepper(3200, step_, dir_);
ZMPT101B voltageSensor(A3, 327.579, 1.34);

EthernetServer server(80);
// Задаём MAC-адрес устройству. Главное, чтобы в сети не было уже зарегистрированно устройство с таким адресом
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
double U_set = 0.0;
double U_current = 0.0;
long int value = 0;
static uint32_t tmr2;
static uint32_t tmr1;

#include "support_functions.h"


void setup() {
  Serial.begin(115200);
  
  stepper.setRunMode(FOLLOW_POS);  
  stepper.setMaxSpeed(100);         // установка макс. скорости в шагах/сек
  stepper.setAcceleration(200);     // установка ускорения в шагах/сек/сек
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
  
  DiscretStep();
 
  SerialParser();
  EthernetParser(); 
 
  PrintData(); 
}
