#include "DHT.h"

// -------------------------
// DHT Configuration
// -------------------------
#define DHTPIN 26           // DHT22 data pin
#define DHTTYPE DHT22       // Sensor type
#define DHT_POWER_PIN 15    // Connect DHT22 VCC here (not 3.3V!)

DHT dht(DHTPIN, DHTTYPE);

// -------------------------
// Struct and Cached Data
// -------------------------

static DHTData lastValidData = {25.0, 50.0};  // Safe default startup values

// -------------------------
// Initialization
// -------------------------
void dht_sensor_init() {
  pinMode(DHT_POWER_PIN, OUTPUT);
  digitalWrite(DHT_POWER_PIN, HIGH);  // Power ON sensor
  delay(1000);
  dht.begin();
  Serial.println("🌡️ DHT Sensor Initialized");
}

// -------------------------
// Power Cycle Function
// -------------------------
void dht_power_cycle() {
  Serial.println("🔄 Power cycling DHT sensor...");
  digitalWrite(DHT_POWER_PIN, LOW);   // Cut power
  delay(1000);
  digitalWrite(DHT_POWER_PIN, HIGH);  // Restore power
  delay(2000);                        // Allow to stabilize
  dht.begin();
  Serial.println("✅ DHT Power Restored & Reinitialized");
}

// -------------------------
// Main DHT Read Function
// -------------------------
DHTData dht_sensor_check() {
  DHTData data;

  // --- Try reading normally ---
  data.humidity = dht.readHumidity();
  data.temperature = dht.readTemperature();

  // --- If reading fails ---
  if (isnan(data.humidity) || isnan(data.temperature)) {
    Serial.println("⚠️ DHT read failed → performing immediate power cycle...");
    dht_power_cycle();

    // Retry once after power restore
    data.humidity = dht.readHumidity();
    data.temperature = dht.readTemperature();
  }

  // --- Still bad? Keep previous valid values ---
  if (isnan(data.humidity) || isnan(data.temperature)) {
    Serial.println("❌ DHT still invalid. Using last valid data.");
    data = lastValidData;
  } else {
    // Cache good readings
    lastValidData = data;
  }

  return data;
}
