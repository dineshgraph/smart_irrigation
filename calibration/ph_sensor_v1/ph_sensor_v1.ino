// 1.25 v for PH 9  --> Alkali
// 1.65 v for ph 6.86 ~ 7  --> Neutral 
// 2.18 v for ph 4 --> Acid


const int phPin = 35;       // Analog input pin

// ===== Arduino Setup =====
void setup() {
  Serial.begin(115200);
  delay(1000);a
  Serial.println("Initializing pH Sensor...");
  pinMode(phPin, INPUT);
  Serial.println("pH Sensor Initialized");

}


void loop() {
  // Convert to voltage (12-bit ADC: 0–4095, 3.3V reference)

  int sensorValue = analogRead(phPin);

  float voltage = sensorValue * (3.3 / 4095.0);   // 1.65 is the base PH 7 Solution. 

 

  Serial.println(voltage);
  
  delay(2000);  // Wait 2 seconds before next reading
}
