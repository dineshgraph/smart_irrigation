#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Small bitmap (16x16) plant icon (you can replace later)
const unsigned char PROGMEM plant_icon[] = {
  0x00,0x00,0x00,0x70,0x00,0xF8,0x01,0xFC,
  0x03,0xFE,0x07,0xEE,0x0F,0xC6,0x1F,0x83,
  0x3F,0x01,0x7E,0x03,0x3E,0x07,0x1C,0x0E,
  0x00,0x1C,0x00,0x38,0x00,0x00,0x00,0x00
};

void oled_init() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.display();
}

void oled_showStartup() {
  display.clearDisplay();
  display.drawBitmap(56, 0, plant_icon, 16, 16, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println("Welcome to Smart");
  display.setCursor(18, 32);
  display.println("Agriculture System");
  display.display();
  delay(2500);
}

void oled_showWifiConnecting(const char* ssid, const char* password) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Connecting WiFi...");
  display.setCursor(0, 26);
  display.print("SSID: ");
  display.println(ssid);
  
  // Optional: show password for debugging (comment out in final version)
  display.setCursor(0, 38);
  display.print("PWD: ");
  display.println(password);

  display.display();
}


void oled_showWifiConnected(IPAddress ip) {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.setTextSize(1);
  display.println("WiFi Connected!");
  display.setCursor(0, 28);
  display.print("IP: ");
  display.println(ip);
  display.display();
  delay(2000);
}

void oled_showSensorInit() {
  display.clearDisplay();
  display.setCursor(0, 20);
  display.setTextSize(1);
  display.println("Initializing Sensors...");
  display.display();
  delay(1500);
  
  display.clearDisplay();
  display.setCursor(15, 24);
  display.println("Sensors Ready!");
  display.display();
  delay(1000);
}

void oled_showMQTTConnecting(const char* server, int port) {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.setTextSize(1);
  display.println("Connecting MQTT...");
  display.setCursor(0, 28);
  display.print("IP: ");
  display.println(server);
  display.setCursor(0, 40);
  display.print("Port: ");
  display.println(port);
  display.display();
}

void oled_showMQTTConnected() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 22);
  display.println("System Active");
  display.setCursor(15, 36);
  display.println("Monitoring Sensors...");
  display.display();
}

void oled_showSensorData(float ph, float soilTemp, int moisture, const char* light, const char* gas,
                         float airTemp, float hum) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("pH:");
  display.print(ph, 1);
  display.print("  M:");
  display.print(moisture);
  display.println("%");
  
  display.setCursor(0, 12);
  display.print("SoilT:");
  display.print(soilTemp, 1);
  display.print("C L:");
  display.println(light);

  display.setCursor(0, 24);
  display.print("Gas:");
  display.println(gas);

  display.setCursor(0, 36);
  display.print("AirT:");
  display.print(airTemp, 1);
  display.print("C  H:");
  display.print(hum, 0);
  display.println("%");

  display.display();
}

void oled_flashPublishIcon() {
  display.fillRect(120, 0, 8, 8, WHITE);  // small square top-right
  display.display();
  delay(100);
  display.fillRect(120, 0, 8, 8, BLACK);
  display.display();
}