// LDR (Light Sensor) Module
const int ldrPin = 12;  // Connect DO pin of LDR module to GPIO12

// Initialize LDR sensor
void ldr_sensor_init() {
  pinMode(ldrPin, INPUT);
  Serial.println("LDR Sensor Initialized");
}

const char* ldr_sensor_check() {
  int lightState = digitalRead(ldrPin);

  if (lightState == HIGH) {
    return "dark";
  } else {
    return "light";
  }
}

