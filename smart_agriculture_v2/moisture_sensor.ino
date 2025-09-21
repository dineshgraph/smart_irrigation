/*
  ESP32 Soil Moisture Sensor
  http:://www.electronicwings.com
*/

int _moisture,sensor_analog;
const int sensor_pin = 34;  // ADC1, input-only// 36;  /* Soil moisture sensor O/P pin  36 // VP A0 */


void moisture_sensor_init() {
  temp_sensors.begin();
}

void moisture_sensor_check() {
  sensor_analog = analogRead(sensor_pin);
  _moisture = ( 100 - ( (sensor_analog/4095.00) * 100 ) );
  Serial.print("Moisture = ");
  Serial.print(_moisture);  /* Print Temperature on the serial window */
  Serial.println("%");
}
