/*
 * ESP32 IoT AQI Monitoring System
 * 
 * Sensors:
 * - SDS011: PM2.5, PM10 (UART)
 * - DHT11: Temperature, Humidity (Digital)
 * - MQ135: Gas/PPM (Analog)
 * - BME680: Pressure, Altitude, Gas, Temperature (I2C)
 * 
 * Transmission: MQTT to broker, optional InfluxDB
 * 
 * License: MIT
 * Version: 1.0.0
 */

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Adafruit_BME680.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>

#include "config.h"

// ===== HARDWARE OBJECTS =====
HardwareSerial SDS_SERIAL(1);  // UART1 for SDS011
DHT dht(DHT_PIN, DHT11);
Adafruit_BME680 bme;
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ===== DATA STRUCTURE =====
struct AQIReading {
  // SDS011 - Particulate Matter
  float pm25;
  float pm10;
  
  // DHT11 - Temperature & Humidity
  float temp_dht;
  float humidity;
  
  // BME680 - Environmental
  float temp_bme;
  float pressure;
  float altitude;
  float gas_resistance;
  
  // MQ135 - Gas/PPM
  int ppm;
  
  // Metadata
  uint32_t timestamp;
  uint8_t quality_flag;  // Data quality indicator
};

// ===== GLOBAL VARIABLES =====
AQIReading currentReading;
unsigned long lastMeasurementTime = 0;
unsigned long lastMQTTTime = 0;
uint32_t bootTimestamp = 0;

// ===== FUNCTION PROTOTYPES =====
void setup_wifi();
void reconnect_mqtt();
void callback(char* topic, byte* payload, unsigned int length);
void acquire_all_sensors();
void read_sds011();
void read_dht11();
void read_mq135();
void read_bme680();
void publish_mqtt_data();
void publish_influxdb_data();
void display_measurements();
void check_alert_thresholds();

// ===== SETUP FUNCTION =====
void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(2000);
  
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║  ESP32 IoT AQI Monitoring System   ║");
  Serial.println("║  v1.0.0 - Production Ready         ║");
  Serial.println("║  SDS011 + DHT11 + MQ135 + BME680   ║");
  Serial.println("╚════════════════════════════════════╝\n");
  
  bootTimestamp = time(NULL);
  
  // Initialize DHT11
  Serial.print("[Init] DHT11...");
  dht.begin();
  Serial.println(" OK");
  
  // Initialize SDS011 (UART1)
  Serial.print("[Init] SDS011 (UART1)...");
  SDS_SERIAL.begin(9600, SERIAL_8N1, SDS_RX_PIN, SDS_TX_PIN);
  Serial.println(" OK");
  
  // Initialize BME680 (I2C)
  Serial.print("[Init] BME680 (I2C)...");
  if (!bme.begin(BME680_ADDR)) {
    Serial.println(" FAILED!");
  } else {
    Serial.println(" OK");
    // Configure BME680
    bme.setTemperatureOversampling(OSR_2X);
    bme.setPressureOversampling(OSR_4X);
    bme.setHumidityOversampling(OSR_2X);
    bme.setIIRFilterSize(IIR_FILTER_SIZE_3);
    bme.setGasHeater(320, 150);  // 320°C for 150ms
  }
  
  // WiFi
  setup_wifi();
  
  // MQTT
  if (ENABLE_MQTT) {
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(callback);
  }
  
  Serial.println("\n[Status] System Ready\n");
}

// ===== MAIN LOOP =====
void loop() {
  // WiFi keep-alive
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Connection lost, reconnecting...");
    setup_wifi();
  }
  
  // MQTT keep-alive
  if (ENABLE_MQTT) {
    if (!mqttClient.connected()) {
      reconnect_mqtt();
    }
    mqttClient.loop();
  }
  
  // Measurement cycle
  if (millis() - lastMeasurementTime >= (MEASUREMENT_INTERVAL * 1000)) {
    lastMeasurementTime = millis();
    
    Serial.println("\n[Cycle] Acquiring sensor data...");
    acquire_all_sensors();
    display_measurements();
    check_alert_thresholds();
    
    // Transmit
    if (ENABLE_MQTT) {
      publish_mqtt_data();
    }
    if (INFLUXDB_ENABLED) {
      publish_influxdb_data();
    }
  }
  
  delay(100);
}

// ===== WIFI SETUP =====
void setup_wifi() {
  Serial.print("[WiFi] Connecting to '");
  Serial.print(WIFI_SSID);
  Serial.print("'...");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.isConnected()) {
    Serial.print(" OK\n[WiFi] IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("[WiFi] Signal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println(" FAILED (will retry)");
  }
}

// ===== MQTT RECONNECT =====
void reconnect_mqtt() {
  if (!mqttClient.connected()) {
    Serial.print("[MQTT] Connecting to ");
    Serial.print(MQTT_SERVER);
    Serial.print(":");
    Serial.print(MQTT_PORT);
    Serial.print("...");
    
    if (mqttClient.connect(STATION_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println(" OK");
      
      // Subscribe to topics
      mqttClient.subscribe(MQTT_TOPIC_CONFIG);
      mqttClient.subscribe(MQTT_TOPIC_STATUS);
      
      // Publish online status
      mqttClient.publish(MQTT_TOPIC_STATUS, "online");
    } else {
      Serial.print(" FAILED (rc=");
      Serial.print(mqttClient.state());
      Serial.println(")");
    }
  }
}

// ===== MQTT CALLBACK =====
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("[MQTT] Message received: ");
  Serial.println(topic);
}

// ===== ACQUIRE ALL SENSORS =====
void acquire_all_sensors() {
  read_dht11();
  read_mq135();
  read_sds011();
  read_bme680();
  
  currentReading.timestamp = time(NULL);
  currentReading.quality_flag = 0;  // 0 = valid, >0 = error code
}

// ===== READ SDS011 PARTICULATE SENSOR =====
void read_sds011() {
  Serial.println("[SDS011] Reading PM2.5, PM10...");
  
  unsigned char buf[32];
  int idx = 0;
  unsigned long timeout = millis() + 5000;
  
  // Read frame until sync bytes found or timeout
  while (millis() < timeout && idx < 32) {
    if (SDS_SERIAL.available()) {
      unsigned char c = SDS_SERIAL.read();
      
      // Look for frame sync bytes (0x42 0x4d)
      if (idx == 0 && c != 0x42) continue;
      if (idx == 1 && c != 0x4d) {idx = 0; continue;}
      
      buf[idx++] = c;
    }
  }
  
  if (idx == 32) {
    // Extract PM values (bytes 10-13)
    currentReading.pm25 = ((buf[10] << 8) | buf[11]) / 10.0 + SDS_PM_OFFSET;
    currentReading.pm10 = ((buf[12] << 8) | buf[13]) / 10.0 + SDS_PM_OFFSET;
    
    Serial.print("  PM2.5: ");
    Serial.print(currentReading.pm25, 1);
    Serial.print(" μg/m³  |  PM10: ");
    Serial.print(currentReading.pm10, 1);
    Serial.println(" μg/m³");
  } else {
    Serial.println("  ERROR: Failed to read frame");
    currentReading.quality_flag = 1;
  }
}

// ===== READ DHT11 SENSOR =====
void read_dht11() {
  Serial.println("[DHT11] Reading Temperature, Humidity...");
  
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (!isnan(temp) && !isnan(humidity)) {
    currentReading.temp_dht = temp + DHT_TEMP_OFFSET;
    currentReading.humidity = humidity + DHT_HUMIDITY_OFFSET;
    
    Serial.print("  Temperature: ");
    Serial.print(currentReading.temp_dht, 1);
    Serial.print("°C  |  Humidity: ");
    Serial.print(currentReading.humidity, 1);
    Serial.println("%");
  } else {
    Serial.println("  ERROR: Failed to read");
    currentReading.quality_flag |= 2;
  }
}

// ===== READ MQ135 GAS SENSOR =====
void read_mq135() {
  Serial.println("[MQ135] Reading Gas/PPM...");
  
  int raw = analogRead(MQ135_PIN);
  currentReading.ppm = (int)(map(raw, 0, 4095, 0, 500) * MQ135_CALIBRATION);
  
  Serial.print("  PPM: ");
  Serial.print(currentReading.ppm);
  Serial.print("  (raw: ");
  Serial.print(raw);
  Serial.println(")");
}

// ===== READ BME680 ENVIRONMENTAL SENSOR =====
void read_bme680() {
  Serial.println("[BME680] Reading Pressure, Altitude, Gas...");
  
  if (bme.performReading()) {
    currentReading.temp_bme = bme.temperature;
    currentReading.pressure = bme.pressure / 100.0;  // Convert to hPa
    currentReading.altitude = bme.readAltitude(1013.25);
    currentReading.gas_resistance = bme.gas_resistance;
    
    Serial.print("  Pressure: ");
    Serial.print(currentReading.pressure, 1);
    Serial.print(" hPa  |  Altitude: ");
    Serial.print(currentReading.altitude, 1);
    Serial.print(" m  |  Gas: ");
    Serial.print(currentReading.gas_resistance, 0);
    Serial.println(" Ω");
  } else {
    Serial.println("  ERROR: Failed to read");
    currentReading.quality_flag |= 4;
  }
}

// ===== PUBLISH TO MQTT =====
void publish_mqtt_data() {
  if (!mqttClient.connected()) return;
  
  DynamicJsonDocument doc(512);
  doc["station_id"] = STATION_ID;
  doc["station_name"] = STATION_NAME;
  doc["timestamp"] = currentReading.timestamp;
  doc["uptime_sec"] = (uint32_t)(millis() / 1000);
  
  // Measurements object
  JsonObject measurements = doc.createNestedObject("measurements");
  measurements["pm25"] = round(currentReading.pm25 * 10) / 10.0;
  measurements["pm10"] = round(currentReading.pm10 * 10) / 10.0;
  measurements["temperature"] = round(currentReading.temp_dht * 10) / 10.0;
  measurements["humidity"] = round(currentReading.humidity * 10) / 10.0;
  measurements["pressure"] = round(currentReading.pressure * 100) / 100.0;
  measurements["altitude"] = round(currentReading.altitude * 10) / 10.0;
  measurements["gas_resistance"] = currentReading.gas_resistance;
  measurements["ppm"] = currentReading.ppm;
  
  // Quality info
  JsonObject quality = doc.createNestedObject("quality");
  quality["flag"] = currentReading.quality_flag;
  quality["wifi_rssi"] = WiFi.RSSI();
  
  String payload;
  serializeJson(doc, payload);
  
  if (mqttClient.publish(MQTT_TOPIC_DATA, payload.c_str())) {
    Serial.println("[MQTT] Published successfully");
  } else {
    Serial.println("[MQTT] Publish failed");
  }
}

// ===== PUBLISH TO INFLUXDB (Placeholder) =====
void publish_influxdb_data() {
  // TODO: Implement InfluxDB HTTP line protocol
  // Example: "measurement,tag=value field=1.0"
  Serial.println("[InfluxDB] Integration ready (placeholder)");
}

// ===== DISPLAY MEASUREMENTS =====
void display_measurements() {
  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║           CURRENT MEASUREMENTS             ║");
  Serial.println("╠════════════════════════════════════════════╣");
  Serial.print("║ PM2.5:          ");
  Serial.print(currentReading.pm25, 1);
  Serial.println(" μg/m³                        ║");
  Serial.print("║ PM10:           ");
  Serial.print(currentReading.pm10, 1);
  Serial.println(" μg/m³                        ║");
  Serial.print("║ Temperature:    ");
  Serial.print(currentReading.temp_dht, 1);
  Serial.println("°C                           ║");
  Serial.print("║ Humidity:       ");
  Serial.print(currentReading.humidity, 1);
  Serial.println("%                            ║");
  Serial.print("║ Pressure:       ");
  Serial.print(currentReading.pressure, 1);
  Serial.println(" hPa                        ║");
  Serial.print("║ Altitude:       ");
  Serial.print(currentReading.altitude, 1);
  Serial.println(" m                           ║");
  Serial.print("║ Gas Resistance: ");
  Serial.print(currentReading.gas_resistance, 0);
  Serial.println(" Ω                          ║");
  Serial.print("║ PPM:            ");
  Serial.print(currentReading.ppm);
  Serial.println("                             ║");
  Serial.print("║ WiFi Signal:    ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm                        ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
}

// ===== CHECK ALERT THRESHOLDS =====
void check_alert_thresholds() {
  if (currentReading.pm25 > PM25_ALERT_THRESHOLD) {
    Serial.print("[ALERT] PM2.5 HIGH: ");
    Serial.print(currentReading.pm25);
    Serial.println(" μg/m³");
  }
  
  if (currentReading.pm10 > PM10_ALERT_THRESHOLD) {
    Serial.print("[ALERT] PM10 HIGH: ");
    Serial.print(currentReading.pm10);
    Serial.println(" μg/m³");
  }
}
