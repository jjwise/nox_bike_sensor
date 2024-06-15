#include <Wire.h>
#include "MICS6814.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define PIN_CO A2
#define PIN_NO2 A0
#define PIN_NH3 A1

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C
MICS6814 gas(PIN_CO, PIN_NO2, PIN_NH3);

void setup()
{
  Serial.begin(9600);

  gas.beginInitialCalibration();

  if (!bme.begin())
  {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void loop()
{
  unsigned long endTime = bme.beginReading();

  delay(50); // This represents parallel work.

  if (endTime == 0 || !bme.endReading())
  {
    Serial.println(F("Failed to complete BME680"));
  } else {
    Serial.print(F("Temperature = "));
    Serial.print(bme.temperature);
    Serial.println(F(" *C"));

    Serial.print(F("Pressure = "));
    Serial.print(bme.pressure / 100.0);
    Serial.println(F(" hPa"));

    Serial.print(F("Humidity = "));
    Serial.print(bme.humidity);
    Serial.println(F(" %"));

    Serial.print(F("Gas = "));
    Serial.print(bme.gas_resistance / 1000.0);
    Serial.println(F(" KOhms"));

    Serial.print(F("Approx. Altitude = "));
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(F(" m"));
  }

  gas.updateMovingAvgFilter();

  Serial.print("NO2 R0: ");
  Serial.print(gas.R0.NO2);
  Serial.println("ohms");

  Serial.print("CO R0: ");
  Serial.print(gas.R0.CO);
  Serial.println("ohms");

  Serial.print("NH3 R0: ");
  Serial.print(gas.R0.NH3);
  Serial.println("ohms");

  delay(100);
}