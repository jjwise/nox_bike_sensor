#ifndef MICS6814_H
#define MICS6814_H

#define MOVING_AVERAGE_FILTER_SIZE 100
#define SENSE_LOAD_RESISTANCE 51000 //ohms
#define VCC 5.0 //V
#define ADC_RESOLUTION 1023.0

#include "Arduino.h"

typedef struct
{
	uint16_t CO;
	uint16_t NO2;
	uint16_t NH3;
} gas_r0;

// struct to store moving average filter state
typedef struct
{
	gas_r0 *filterArray;
	gas_r0 filterSum;
	gas_r0 filterAverage;
	uint16_t filterSize;
	uint16_t filterIndex;
} moving_average_filter;

class MICS6814
{
public:
	MICS6814(int pinCO, int pinNO2, int pinNH3);

	void beginInitialCalibration();
	void updateMovingAvgFilter();

	gas_r0 R0;

private:
	void _updateR0FromMovingAvgFilter();

	uint8_t _pinCO;
	uint8_t _pinNO2;
	uint8_t _pinNH3;

	moving_average_filter _moving_average_filter;
};

#endif