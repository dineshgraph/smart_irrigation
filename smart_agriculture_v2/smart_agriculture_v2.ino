#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ===== WiFi & MQTT Configuration =====
const char* ssid = "keto's lab";
const char* password = "keto90666";
const char* mqtt_server = "192.168.0.97"; // Replace with your MQTT broker IP
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// ===== MQTT Topics =====
const char* TOPIC_GAS = "sensors/gas";
const char* TOPIC_SOIL_TEMP = "sensors/soil_temperature";
const char* TOPIC_MOISTURE = "sensors/soil_moisture";
const char* TOPIC_PH = "sensors/ph";
const char* TOPIC_LIGHT = "sensors/light";
const char* TOPIC_AIR_TEMP = "sensors/air_temperature";
const char* TOPIC_AIR_HUM = "sensors/air_humidity";


// Struct to hold both values
struct DHTData {
  float temperature;
  float humidity;
};


// ===== WiFi Connection =====
void setup_wifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected ✅");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// ===== MQTT Reconnect =====
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to ");
    Serial.print(mqtt_server);
    Serial.print(":");
    Serial.println(mqtt_port);

    if (client.connect("ESP32Client")) {
      Serial.println("connected ✅");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5s");
      delay(5000);
    }
  }
}



// ===== Publish JSON with string value =====
void publish_sensor_json(const char* topic, const char* sensor_id, const char* sensor_type, const char* valueStr, const char* unit, const char* location) {
  StaticJsonDocument<256> doc;
  

  doc["sensor_id"] = sensor_id;
  doc["sensor_type"] = sensor_type;
  doc["value"] = valueStr;  // Always string
  doc["unit"] = unit;
  doc["location"] = location;

  char payload[256];
  serializeJson(doc, payload);

  client.publish(topic, payload);
  Serial.print("Published → "); Serial.print(topic); Serial.print(": "); Serial.println(payload);
}

// ===== Global buffer =====
char payload[20]; // buffer for converting numbers to strings

// ===== Setup =====
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  // analogReadResolution(10);   // Use 10-bit ADC resolution
  Serial.println("Smart Agriculture v2 Initialized!");



  // Initialize sensors
  ph_sensor_init();
  soil_temp_sensor_init();
  moisture_sensor_init(); 
  ldr_sensor_init();
  mq_sensor_init();
  dht_sensor_init();
  oled_init();
  
}

// ===== Main Loop =====
void loop() {
  // Ensure MQTT connection
  // if (!client.connected()) {
  //   reconnect();
  // }
  // client.loop();
  
  float phValue = ph_sensor_check(); // 0 - 14 (0 - acid) (7 - neutral) (14 - Alkali)
  float soiltempC = soil_temperature_sensor_check();
  int soilMoisture = moisture_sensor_check(); // in % 
  const char* lightStatus = ldr_sensor_check(); // "light"/"dark" 
  const char* gasStatus = mq_sensor_check(); // ""toxic_gas"/""normal_gas""
  DHTData env = dht_sensor_check();


  Serial.print("PH: ");
  Serial.println(phValue);

  Serial.print("Soil_Temp_C: "); // celsius
  Serial.println(soiltempC);

  Serial.print("Soil_Moisture: "); // celsius
  Serial.println(soilMoisture);

  Serial.print("Light_status: "); // celsius
  Serial.println(lightStatus);

  Serial.print("GasStatus: "); // 
  Serial.println(gasStatus);

  if (env.temperature != -1) {
    Serial.print("Air Temperature: ");
    Serial.print(env.temperature);
    Serial.println(" °C");

    Serial.print("Air Humidity: ");
    Serial.print(env.humidity);
    Serial.println(" %");
  }
  Serial.println("=============================================");


  // ===== Convert and Publish all as strings =====
  // sprintf(payload, "%d", gas);
  // publish_sensor_json(TOPIC_GAS, "gas_001", "gas_sensor", payload, "ppm", "field_1"); //parts-per-million (ppm). This represents the concentration of the target gas in the air, indicating how many molecules of the sensing gas

  // dtostrf(soiltempC, 4, 2, payload);
  // publish_sensor_json(TOPIC_SOIL_TEMP , "temp_001", "soil_temp_sensor", payload, "celsius", "field_1");

  // sprintf(payload, "%d", moisture);
  // publish_sensor_json(TOPIC_MOISTURE, "moist_001", "moisture_sensor", payload, "percent", "field_1");

  // dtostrf(phValue, 4, 2, payload);
  // publish_sensor_json(TOPIC_PH, "ph_001", "ph_sensor", payload, "pH", "field_1");

  // publish_sensor_json(TOPIC_LIGHT, "light_001", "light_sensor", lightStatus, "state", "field_1");

  // publish_sensor_json(TOPIC_AIR_TEMP , "temp_002", "air_temp_sensor", "22", "celsius", "field_1"); // Air Temperature

  // publish_sensor_json(TOPIC_AIR_HUM , "moist_002", "air_hum_sensor", "45" , "percent", "field_1"); // Air Humidity

  // delay(20000); // send data every 20 seconds
}
