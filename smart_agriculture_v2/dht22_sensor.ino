#include "DHT.h"

#define DHTPIN 5
#define DHTTYPE DHT22   // DHT22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);


void dht_sensor_init() {
  dht.begin();
  Serial.println("DHT Sensor Initialized");
}

DHTData dht_sensor_check() {
  DHTData data;

  delay(2000); // Wait between measurements (DHT is slow)

  data.humidity = dht.readHumidity();
  data.temperature = dht.readTemperature(); // °C

  // Validate readings
  if (isnan(data.humidity) || isnan(data.temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    data.humidity = -1;
    data.temperature = -1;
    return data;
  }

  // Print readings
  // Serial.print("Humidity: ");
  // Serial.print(data.humidity);
  // Serial.print("%  Temperature: ");
  // Serial.print(data.temperature);
  // Serial.println("°C");

  return data;
}
