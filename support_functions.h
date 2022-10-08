
void GoToTarget(long int value) {
  if (value != stepper.getTarget()) {
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

int CountSteps(double rhs, double lhs) {
  return abs(rhs - lhs)*100/30;
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
