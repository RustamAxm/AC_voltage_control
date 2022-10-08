#pragma once

#include <Arduino.h>

#define ADC_SCALE 1023.0
#define VREF 5.0
#define DEFAULT_FREQUENCY 50


class ZMPT101B {
public:
	ZMPT101B(uint8_t _pin, float a, float b);
	int calibrate();
	void setZeroPoint(int _zero);
	void setSensitivity(float sens);
	float getVoltageDC();
	float getVoltageAC(uint16_t frequency = 50);
	double getVoltageAC_custom(int counts = 2000);

private:
	int zero = 512;
	float sensitivity;
	uint8_t pin;
  float a_ = 0.0;
  float b_ = 0.0;
};
