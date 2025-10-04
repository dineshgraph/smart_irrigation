int _moisture, sensor_analog;
const int sensor_pin = 34;  // ADC1 pin

// Calibrated limits (adjust if needed)
const int sensor_dry = 4095;  // 0% moisture (dry)
const int sensor_wet = 350;   // 100% moisture (wet)

void moisture_sensor_init() {
  pinMode(sensor_pin, INPUT);
  Serial.println("Moisture Sensor Initialized");
}

int moisture_sensor_check() {
  int readings[10];

  // Take 10 readings
  for (int i = 0; i < 10; i++) {
    readings[i] = analogRead(sensor_pin);
    delay(20);  // small delay between readings
  }

  // Sort readings (simple bubble sort)
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (readings[i] > readings[j]) {
        int temp = readings[i];
        readings[i] = readings[j];
        readings[j] = temp;
      }
    }
  }

  // Average the middle 6 readings (remove outliers)
  long sum = 0;
  for (int i = 2; i < 8; i++) {  // indices 2–7
    sum += readings[i];
  }
  sensor_analog = sum / 6;

  // Convert to % using calibration map
  _moisture = map(sensor_analog, sensor_dry, sensor_wet, 0, 100);
  _moisture = constrain(_moisture, 0, 100);

  // Print results
  // Serial.print("Moisture Analog Avg: ");
  // Serial.print(sensor_analog);
  // Serial.print(" -> Moisture: ");
  // Serial.print(_moisture);
  // Serial.println("%");

  return _moisture;
}
