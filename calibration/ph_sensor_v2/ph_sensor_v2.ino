// ===== pH Sensor Reference Values =====
// 1.25V → pH 9   (Alkaline)
// 1.65V → pH 7   (Neutral)
// 2.18V → pH 4   (Acidic)

const int phPin = 35;  // Analog input pin (ESP32 ADC1_CH7)

// ===== Setup =====
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Initializing pH Sensor...");
  pinMode(phPin, INPUT);
  Serial.println("pH Sensor Initialized");
}

// ===== Loop =====
void loop() {
  int buf[10], temp;
  unsigned long avgValue = 0;

  // Step 1: Take 10 readings
  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(phPin);
    delay(10);  // small delay between samples
  }

  // Step 2: Sort readings (simple bubble sort)
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  // Step 3: Average middle 6 values
  for (int i = 2; i < 8; i++) {
    avgValue += buf[i];
  }
  float avgADC = avgValue / 6.0;

  // Step 4: Convert ADC value → voltage (ESP32 = 12-bit ADC, 3.3V reference)
  float voltage = avgADC * (3.3 / 4095.0);

  // Step 5: Convert voltage → pH (linear interpolation)
  // Using the reference points: (1.25V, 9), (1.65V, 7), (2.18V, 4)
  // Slope = (4 - 9) / (2.18 - 1.25) = -5 / 0.93 ≈ -5.38
  // Intercept = 9 - (-5.38 * 1.25) = 15.73
  float phValue = -5.38 * voltage + 15.73;

  // Step 6: Print result
  Serial.print("Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V  |  pH: ");
  Serial.println(phValue, 2);

  delay(2000);  // 2 seconds between readings
}
