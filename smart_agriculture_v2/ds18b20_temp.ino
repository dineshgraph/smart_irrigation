#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected
const int oneWireBus = 4;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
DallasTemperature temp_sensors(&oneWire);

void soil_temp_sensor_init() {
  temp_sensors.begin();
}


float soil_temperature_sensor_check() {
  temp_sensors.requestTemperatures(); 
  float soil_temperatureC = temp_sensors.getTempCByIndex(0);
  // float temperatureF = temp_sensors.getTempFByIndex(0); // optional if you don’t use it
  return soil_temperatureC;
}

