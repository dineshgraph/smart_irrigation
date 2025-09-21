// Robocraze Analog pH Sensor Kit
// ESP32 analog input pin: GPIO35 (ADC1 input-only)
// Output: Serial Monitor

const int phPin = 35;       // ESP32 analog pin connected to pH sensor Po
float calibration = 0.0;    // Adjust this after calibration for accurate pH

int buffer[10];
unsigned long avgValue;
int temp;

// Initialize pH sensor (no special setup needed for analog)
void ph_sensor_init() {
  // Nothing to initialize for analog pin
}

// Read pH sensor and print to Serial Monitor
void ph_sensor_check() {
  // Take 10 samples
  for (int i = 0; i < 10; i++) {
    buffer[i] = analogRead(phPin);
    delay(30);
  }

  // Sort buffer values (bubble sort)
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer[i] > buffer[j]) {
        temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
      }
    }
  }

  // Average middle 6 values
  avgValue = 0;
  for (int i = 2; i < 8; i++) avgValue += buffer[i];

  // Convert to voltage (ESP32 ADC 12-bit: 0-4095, 3.3V reference)
  float voltage = (float)avgValue * 3.3 / 4095.0 / 6.0;

  // Calculate pH value using Robocraze formula
  float phValue = -5.70 * voltage + calibration;

  // Print to Serial Monitor
  Serial.print("pH Value: ");
  Serial.println(phValue, 2);
}
