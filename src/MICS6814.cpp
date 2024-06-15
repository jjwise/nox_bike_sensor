#include "MICS6814.h"

MICS6814::MICS6814(int pinCO, int pinNO2, int pinNH3)
{
    _pinCO = pinCO;
    _pinNO2 = pinNO2;
    _pinNH3 = pinNH3;
}

void MICS6814::beginInitialCalibration()
{
    // Initialise moving average filter state
    _moving_average_filter = {
        new gas_r0[MOVING_AVERAGE_FILTER_SIZE],
        {0, 0, 0},
        {0, 0, 0},
        MOVING_AVERAGE_FILTER_SIZE,
        0
    };
}

void MICS6814::updateMovingAvgFilter()
{
    // Read sensor values
    gas_r0 _current_gas_r0 = {
        analogRead(_pinCO),
        analogRead(_pinNO2),
        analogRead(_pinNH3)
    };

    // Update the filter state with the new sensor values
    _moving_average_filter.filterSum.CO += _current_gas_r0.CO - _moving_average_filter.filterArray[_moving_average_filter.filterIndex].CO;
    _moving_average_filter.filterSum.NO2 += _current_gas_r0.NO2 - _moving_average_filter.filterArray[_moving_average_filter.filterIndex].NO2;
    _moving_average_filter.filterSum.NH3 += _current_gas_r0.NH3 - _moving_average_filter.filterArray[_moving_average_filter.filterIndex].NH3;

    _moving_average_filter.filterArray[_moving_average_filter.filterIndex] = _current_gas_r0;

    _moving_average_filter.filterAverage.CO = _moving_average_filter.filterSum.CO / MOVING_AVERAGE_FILTER_SIZE;
    _moving_average_filter.filterAverage.NO2 = _moving_average_filter.filterSum.NO2 / MOVING_AVERAGE_FILTER_SIZE;
    _moving_average_filter.filterAverage.NH3 = _moving_average_filter.filterSum.NH3 / MOVING_AVERAGE_FILTER_SIZE;

    _moving_average_filter.filterIndex = (_moving_average_filter.filterIndex + 1) % MOVING_AVERAGE_FILTER_SIZE;

    // Update the real R0 values
    _updateR0FromMovingAvgFilter();
}

void MICS6814::_updateR0FromMovingAvgFilter()
{
    // Update the real R0 values
    float c0_sense_voltage = _moving_average_filter.filterAverage.CO * VCC / ADC_RESOLUTION;
    float no2_sense_voltage = _moving_average_filter.filterAverage.NO2 * VCC / ADC_RESOLUTION;
    float nh3_sense_voltage = _moving_average_filter.filterAverage.NH3 * VCC / ADC_RESOLUTION;

    R0.CO = (c0_sense_voltage * SENSE_LOAD_RESISTANCE) / (VCC - c0_sense_voltage);
    R0.NO2 = (no2_sense_voltage * SENSE_LOAD_RESISTANCE) / (VCC - no2_sense_voltage);
    R0.NH3 = (nh3_sense_voltage * SENSE_LOAD_RESISTANCE) / (VCC - nh3_sense_voltage);
}