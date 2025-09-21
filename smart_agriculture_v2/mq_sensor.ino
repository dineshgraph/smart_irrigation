/*
Link: https://www.devadiy.com
*/

/*
Link: https://www.devadiy.com
*/
// Pin connections
#define MQ2_PIN 13 // Analog input pin for MQ-2 sensor

#define GAS_THRESHOLD 600 // Gas threshold value for warning

// Initialization function for MQ-2 sensor
void mq_sensor_init() {
  pinMode(MQ2_PIN, INPUT);
}

// Function to read sensor value
void mq_sensor_check() {
  int gasValue = analogRead(MQ2_PIN);
  Serial.print("Gas Value: ");
  Serial.println(gasValue);

  if (gasValue > GAS_THRESHOLD) {
    Serial.println("⚠️ Warning! High gas or smoke levels detected!");
  }
}


// Pin connections
// #define MQ2_PIN 13 // Analog input pin for MQ-2 sensor
// #define LEDRED_PIN 12
// #define LEDGREEN_PIN 14

// #define GAS_THRESHOLD 600 // Gas threshold value for warning

// // Initialization function for MQ-2 sensor and LEDs
// void mq_sensor_init() {
//   pinMode(MQ2_PIN, INPUT);
//   pinMode(LEDRED_PIN, OUTPUT);
//   pinMode(LEDGREEN_PIN, OUTPUT);
//   digitalWrite(LEDRED_PIN, LOW);
//   digitalWrite(LEDGREEN_PIN, LOW);
// }

// // Function to read sensor value and control LEDs
// void mq_sensor_check() {
//   int gasValue = analogRead(MQ2_PIN);
//   Serial.print("Gas Value: ");
//   Serial.println(gasValue);

//   if (gasValue > GAS_THRESHOLD) {
//     Serial.println("⚠️ Warning! High gas or smoke levels detected!");
//     digitalWrite(LEDRED_PIN, HIGH);
//     digitalWrite(LEDGREEN_PIN, LOW);
//   } else {
//     digitalWrite(LEDRED_PIN, LOW);
//     digitalWrite(LEDGREEN_PIN, HIGH);
//   }
// }
