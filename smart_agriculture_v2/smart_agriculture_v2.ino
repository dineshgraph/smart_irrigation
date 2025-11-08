#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// --- Forward declarations for DHT sensor (defined in dht22_sensor.ino) ---
struct DHTData {
  float temperature;
  float humidity;
};

void dht_sensor_init();
DHTData dht_sensor_check();
void dht_power_cycle();

// ===== WiFi & MQTT Configuration =====
const char* ssid = "keto's lab";
const char* password = "keto90666";
const char* mqtt_server = "192.168.0.67";
const int mqtt_port = 1883;

// ===== Hardware Pins =====
const int LED_BULB_HIGH_VOLTAGE = 18;
const int MOTOR_PUMP = 19;
const int RED_LED = 27;
const int GREEN_LED = 14;

// ===== MQTT Topics =====
const char* TOPIC_GAS        = "sensors/gas";
const char* TOPIC_SOIL_TEMP  = "sensors/soil_temperature";
const char* TOPIC_MOISTURE   = "sensors/soil_moisture";
const char* TOPIC_PH         = "sensors/ph";
const char* TOPIC_LIGHT      = "sensors/light";
const char* TOPIC_AIR_TEMP   = "sensors/air_temperature";
const char* TOPIC_AIR_HUM    = "sensors/air_humidity";

WiFiClient espClient;
PubSubClient client(espClient);

// ===== Global Timers =====
unsigned long lastOledUpdate = 0;
unsigned long lastPublish = 0;
unsigned long lastSensorRead = 0;

// ===== Intervals =====
const unsigned long SENSOR_READ_INTERVAL = 1000;
const unsigned long OLED_UPDATE_INTERVAL = 1000;
const unsigned long MQTT_PUBLISH_INTERVAL = 20000;

// ===== Cached Sensor Data =====
float phValue = 0;
float soilTempC = 0;
int soilMoisture = 0;
const char* lightStatus = "light";
const char* gasStatus = "normal_gas";
DHTData env = {0, 0};
char payload[32];

// ============================================================
//                  WiFi Connection
// ============================================================
void setup_wifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(GREEN_LED, LOW);
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(GREEN_LED, HIGH);
}

// ============================================================
//                  MQTT Connection
// ============================================================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to ");
    Serial.print(mqtt_server);
    Serial.print(":");
    Serial.println(mqtt_port);

    if (client.connect("ESP32Client")) {
      Serial.println("✅ MQTT connected");
    } else {
      Serial.print("❌ Failed, rc=");
      Serial.print(client.state());
      Serial.println(" — retrying in 5s");
      delay(5000);
    }
  }
}

// ============================================================
//                  JSON Publisher
// ============================================================
void publish_sensor_json(const char* topic, const char* sensor_id, const char* sensor_type,
                         const char* valueStr, const char* unit, const char* location) {
  StaticJsonDocument<256> doc;
  doc["sensor_id"] = sensor_id;
  doc["sensor_type"] = sensor_type;
  doc["value"] = valueStr;
  doc["unit"] = unit;
  doc["location"] = location;

  char jsonPayload[256];
  serializeJson(doc, jsonPayload);

  digitalWrite(RED_LED, HIGH);
  client.publish(topic, jsonPayload);
  delay(150);
  digitalWrite(RED_LED, LOW);

  Serial.print("📤 Published → ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(jsonPayload);
}

// ============================================================
//                  Setup
// ============================================================
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Smart Agriculture v2 Initialized ===");

  pinMode(LED_BULB_HIGH_VOLTAGE, OUTPUT);
  pinMode(MOTOR_PUMP, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  digitalWrite(LED_BULB_HIGH_VOLTAGE, HIGH);
  digitalWrite(MOTOR_PUMP, HIGH);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  // Initialize sensors
  ph_sensor_init();
  soil_temp_sensor_init();
  moisture_sensor_init();
  ldr_sensor_init();
  mq_sensor_init();
  dht_sensor_init();

  // Initialize OLED
  oled_init();
  oled_showStartup();
  oled_showWifiConnecting(ssid, password);
  oled_showWifiConnected(WiFi.localIP());
  oled_showSensorInit();
  oled_showMQTTConnecting(mqtt_server, mqtt_port);
  reconnect();
  oled_showMQTTConnected();
}

// ============================================================
//                  Main Loop
// ============================================================
bool motorState = HIGH;
unsigned long lastRelayToggleTime = 0;
const unsigned long DHT_PROTECT_DELAY = 1500;

void loop() {
  digitalWrite(GREEN_LED, (WiFi.status() == WL_CONNECTED) ? HIGH : LOW);
  if (!client.connected()) reconnect();
  client.loop();

  unsigned long currentMillis = millis();

  // 1️⃣ SENSOR READ
  if (currentMillis - lastSensorRead >= SENSOR_READ_INTERVAL) {
    lastSensorRead = currentMillis;

    phValue      = ph_sensor_check();
    soilTempC    = soil_temperature_sensor_check();
    soilMoisture = moisture_sensor_check();
    lightStatus  = ldr_sensor_check();
    gasStatus    = mq_sensor_check();

    // --- Relay Control for Pump ---
    if (soilMoisture <= 20 && motorState != LOW) {
      motorState = LOW;
      digitalWrite(MOTOR_PUMP, LOW);
      Serial.println("💧 Soil Dry → Motor Pump ON");
      lastRelayToggleTime = currentMillis;
    } else if (soilMoisture > 20 && motorState != HIGH) {
      motorState = HIGH;
      digitalWrite(MOTOR_PUMP, HIGH);
      Serial.println("🪴 Soil Moist → Motor Pump OFF");
      lastRelayToggleTime = currentMillis;
    }

    // --- Relay Control for Light ---
    if (strcmp(lightStatus, "light") == 0) {
      digitalWrite(LED_BULB_HIGH_VOLTAGE, HIGH);
      Serial.println("☀️ Light detected → Turn off bulb");
    } else {
      digitalWrite(LED_BULB_HIGH_VOLTAGE, LOW);
      Serial.println("🌙 Dark detected → Turn on bulb");
    }

    // --- DHT Read ---
    if (currentMillis - lastRelayToggleTime > DHT_PROTECT_DELAY) {
      env = dht_sensor_check();
    } else {
      Serial.println("⏳ Skipping DHT read (relay recently toggled)");
    }

    Serial.println("\n--- Sensor Readings ---");
    Serial.printf("pH Value         : %.2f\n", phValue);
    Serial.printf("Soil Temperature : %.2f °C\n", soilTempC);
    Serial.printf("Soil Moisture    : %d %%\n", soilMoisture);
    Serial.printf("Light Status     : %s\n", lightStatus);
    Serial.printf("Gas Status       : %s\n", gasStatus);
    Serial.printf("Air Temperature  : %.2f °C\n", env.temperature);
    Serial.printf("Air Humidity     : %.2f %%\n", env.humidity);
    Serial.println("------------------------");
  }

  // 2️⃣ OLED DISPLAY UPDATE
  if (currentMillis - lastOledUpdate >= OLED_UPDATE_INTERVAL) {
    lastOledUpdate = currentMillis;
    oled_showSensorData(phValue, soilTempC, soilMoisture, lightStatus,
                        gasStatus, env.temperature, env.humidity);
  }

  // 3️⃣ MQTT PUBLISH
  if (currentMillis - lastPublish >= MQTT_PUBLISH_INTERVAL) {
    lastPublish = currentMillis;

    publish_sensor_json(TOPIC_GAS, "gas_001", "gas_sensor", gasStatus, "state", "field_1");
    dtostrf(soilTempC, 4, 2, payload);
    publish_sensor_json(TOPIC_SOIL_TEMP, "temp_001", "soil_temp_sensor", payload, "celsius", "field_1");
    sprintf(payload, "%d", soilMoisture);
    publish_sensor_json(TOPIC_MOISTURE, "moist_001", "moisture_sensor", payload, "percent", "field_1");
    dtostrf(phValue, 4, 2, payload);
    publish_sensor_json(TOPIC_PH, "ph_001", "ph_sensor", payload, "pH", "field_1");
    publish_sensor_json(TOPIC_LIGHT, "light_001", "light_sensor", lightStatus, "state", "field_1");
    dtostrf(env.temperature, 4, 2, payload);
    publish_sensor_json(TOPIC_AIR_TEMP, "temp_002", "air_temp_sensor", payload, "celsius", "field_1");
    dtostrf(env.humidity, 4, 2, payload);
    publish_sensor_json(TOPIC_AIR_HUM, "moist_002", "air_hum_sensor", payload, "percent", "field_1");

    oled_flashPublishIcon();
    Serial.println("=============================================");
  }
}