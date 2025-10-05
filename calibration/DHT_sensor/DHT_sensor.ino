#include "DHT.h"

// ====== DHT Sensor Configuration ======
#define DHTPIN 5         // DHT sensor connected to GPIO 5
#define DHTTYPE DHT22    // DHT22 (AM2302) sensor

DHT dht(DHTPIN, DHTTYPE);  // Initialize DHT object

// ====== Setup ======
void setup() {
  Serial.begin(115200);
  Serial.println("Initializing DHT Sensor...");

  dht.begin();
  Serial.println("DHT Sensor Initialized Successfully!");
}

// ====== Loop ======
void loop() {
  // DHT22 needs about 2 seconds between reads
  delay(2000);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Default: Celsius

  // Validate readings
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("❌ Failed to read from DHT22 sensor!");
    return;
  }

  // Print sensor data
  Serial.print("🌡 Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C  |  💧 Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
}
