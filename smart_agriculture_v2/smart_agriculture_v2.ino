void setup() {
  Serial.begin(115200);
  analogReadResolution(10);   // Use 10-bit ADC resolution
  Serial.println("Smart Agriculture v2 Initialized!");

  // Initialize sensors
  mq_sensor_init();
  temperature_sensor_init();
  ldr_sensor_init();

}

void loop() {
  // Read and check sensors
  mq_sensor_check();
  temperature_sensor_check();
  moisture_sensor_check();
  ph_sensor_check();
  ldr_sensor_check(); 

  delay(2000);  // Wait 5 seconds before next reading
}
