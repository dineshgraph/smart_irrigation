
// Pin connections
#define MQ2_PIN 13 // Digital pin for MQ-2 sensor


// Initialization function for MQ-2 sensor
void mq_sensor_init() {
  pinMode(MQ2_PIN, INPUT);
  Serial.println("MQ2 Initialized");
}

// Function to read sensor value
const char* mq_sensor_check() {
  int gasValue = digitalRead(MQ2_PIN);

  if (gasValue == HIGH) {
    return "normal_gas";
  } else {
    return "toxic_gas";
  }

}
