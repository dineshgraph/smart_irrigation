// LDR (Light Sensor) Module
const int ldrPin = 12;  // Connect DO pin of LDR module to GPIO12

// Initialize LDR sensor
void ldr_sensor_init() {
  pinMode(ldrPin, INPUT);
  Serial.println("LDR Sensor Initialized");
}

// Check LDR sensor and print state
void ldr_sensor_check() {
  int lightState = digitalRead(ldrPin);

  if (lightState == HIGH) {
    Serial.println("Light Sensor: dark");
  } else {
    Serial.println("Light Sensor: light");
  }
}
