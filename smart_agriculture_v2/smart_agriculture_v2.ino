#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ===== WiFi & MQTT Configuration =====
const char* ssid = "keto's lab";
const char* password = "keto90666";
const char* mqtt_server = "192.168.0.97"; // Replace with your MQTT broker IP (other PC IP)
const int mqtt_port = 1883;

const int LED_BULB_HIGH_VOLTAGE = 18;  // Relay
const int MOTOR_PUMP = 19;              // Relay

// LED indicators
const int RED_LED = 27;   // Turn this LED on on each publish to the MQTT broker
const int GREEN_LED = 14; // Turn this LED on if WiFi is connected, off if disconnected

WiFiClient espClient;
PubSubClient client(espClient);

// ===== MQTT Topics =====
const char* TOPIC_GAS        = "sensors/gas";
const char* TOPIC_SOIL_TEMP  = "sensors/soil_temperature";
const char* TOPIC_MOISTURE   = "sensors/soil_moisture";
const char* TOPIC_PH         = "sensors/ph";
const char* TOPIC_LIGHT      = "sensors/light";
const char* TOPIC_AIR_TEMP   = "sensors/air_temperature";
const char* TOPIC_AIR_HUM    = "sensors/air_humidity";

// ===== Struct to hold DHT data =====
struct DHTData {
  float temperature;
  float humidity;
};

// ===== Global buffer for MQTT payload =====
char payload[32];

// ============================================================
//                  WiFi Connection
// ============================================================
void setup_wifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(GREEN_LED, LOW);  // OFF while connecting
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(GREEN_LED, HIGH); // ON when connected
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
      Serial.println("MQTT connected");
    } else {
      Serial.print("Failed, rc=");
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

  // Turn on RED LED when publishing
  digitalWrite(RED_LED, HIGH);
  client.publish(topic, jsonPayload);
  delay(200);                 // Visible blink delay
  digitalWrite(RED_LED, LOW); // Turn off after message sent

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

  // Ensure relays are OFF initially (active LOW)
  digitalWrite(LED_BULB_HIGH_VOLTAGE, HIGH);
  digitalWrite(MOTOR_PUMP, HIGH);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  // Initialize sensors (your actual init functions)
  ph_sensor_init();
  soil_temp_sensor_init();
  moisture_sensor_init();
  ldr_sensor_init();
  mq_sensor_init();
  dht_sensor_init();
}

// ============================================================
//                  Main Loop
// ============================================================
void loop() {
  // Check WiFi & update LED
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(GREEN_LED, HIGH);
  } else {
    digitalWrite(GREEN_LED, LOW);
  }

  // Ensure MQTT connection
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // ===== Read All Sensor Data =====
  float phValue          = ph_sensor_check();               // 0 - 14 (pH)
  float soilTempC        = soil_temperature_sensor_check(); // °C
  int soilMoisture       = moisture_sensor_check();         // %
  const char* lightStatus = ldr_sensor_check();             // "light"/"dark"
  const char* gasStatus   = mq_sensor_check();              // "toxic_gas"/"normal_gas"
  DHTData env             = dht_sensor_check();             // temp + humidity

  // ===== Serial Debug Output =====
  Serial.println("\n--- Sensor Readings ---");
  Serial.printf("pH Value         : %.2f\n", phValue);
  Serial.printf("Soil Temperature : %.2f °C\n", soilTempC);
  Serial.printf("Soil Moisture    : %d %%\n", soilMoisture);
  Serial.printf("Light Status     : %s\n", lightStatus);
  Serial.printf("Gas Status       : %s\n", gasStatus);

  if (env.temperature != -1) {
    Serial.printf("Air Temperature  : %.2f °C\n", env.temperature);
    Serial.printf("Air Humidity     : %.2f %%\n", env.humidity);
  } else {
    Serial.println("⚠️  DHT Sensor Error");
  }
  Serial.println("------------------------");

  // ============================================================
  // Relay Control Logic for Motor Pump (Active LOW)
  // ============================================================
  if (soilMoisture <= 20) {
    digitalWrite(MOTOR_PUMP, LOW);   // Active LOW → Turn ON motor
    Serial.println("Soil Dry → Motor Pump ON");

    delay(200); // <--- Wait for voltage spike/noise to settle
  } else {
    digitalWrite(MOTOR_PUMP, HIGH);  // Active LOW → Turn OFF motor
    Serial.println("Soil Moist → Motor Pump OFF");
    delay(200);  // <--- Optional, also settle after turning OFF

  }

  // ============================================================
  // Relay Control Logic for Light based on LDR (Active LOW)
  // ============================================================
  if (strcmp(lightStatus, "light") == 0) {
    digitalWrite(LED_BULB_HIGH_VOLTAGE, HIGH);   // Turn Off relay
    Serial.println("Light detected → Turn of Light");
  } else {
    digitalWrite(LED_BULB_HIGH_VOLTAGE, LOW);  // Turn On relay
    Serial.println("Dark detected → Turn on Light");
  }

  // ===== Publish to MQTT =====
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

  Serial.println("=============================================");
  delay(20000); // Publish every 20 seconds
}
