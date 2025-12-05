/*
 * Sensor Test Sketch - Individual Sensor Testing
 * SDS011 + DHT11 + MQ135 + BME680
 */

#include <DHT.h>
#include <Adafruit_BME680.h>
#include <HardwareSerial.h>

// Pin Definitions
#define DHT_PIN 5
#define MQ135_PIN 34
#define SDS_RX_PIN 16
#define SDS_TX_PIN 17

// Objects
HardwareSerial SDS_SERIAL(1);
DHT dht(DHT_PIN, DHT11);
Adafruit_BME680 bme;

void setup() {
  Serial.begin(115200);
  SDS_SERIAL.begin(9600, SERIAL_8N1, SDS_RX_PIN, SDS_TX_PIN);
  dht.begin();
  
  if (!bme.begin(0x77)) {
    Serial.println("BME680 init failed!");
  }
  
  delay(2000);
  
  Serial.println("\n╔═══════════════════════════════╗");
  Serial.println("║   Sensor Test Mode (v1.0)     ║");
  Serial.println("╠═══════════════════════════════╣");
  Serial.println("║ 1 = SDS011 (PM2.5, PM10)      ║");
  Serial.println("║ 2 = DHT11 (Temp, Humidity)    ║");
  Serial.println("║ 3 = MQ135 (Gas/PPM)           ║");
  Serial.println("║ 4 = BME680 (Pressure, etc)    ║");
  Serial.println("║ 5 = ALL SENSORS               ║");
  Serial.println("╚═══════════════════════════════╝\n");
}

void loop() {
  if (Serial.available()) {
    int cmd = Serial.read();
    Serial.print("\n[TEST] Command: ");
    Serial.println(cmd);
    
    switch(cmd) {
      case '1': test_sds011(); break;
      case '2': test_dht11(); break;
      case '3': test_mq135(); break;
      case '4': test_bme680(); break;
      case '5': test_all(); break;
    }
  }
}

// ===== SDS011 PARTICULATE SENSOR =====
void test_sds011() {
  Serial.println("[SDS011] Testing PM sensor (30 sec timeout)...");
  
  unsigned char buf[32];
  int idx = 0;
  unsigned long timeout = millis() + 30000;  // 30 second timeout
  
  Serial.println("Waiting for frame sync (0x42 0x4d)...");
  
  while (millis() < timeout && idx < 32) {
    if (SDS_SERIAL.available()) {
      unsigned char c = SDS_SERIAL.read();
      
      // Look for frame start bytes
      if (idx == 0 && c != 0x42) continue;
      if (idx == 1 && c != 0x4d) {idx = 0; continue;}
      
      buf[idx++] = c;
      
      if (idx % 10 == 0) {
        Serial.print(".");
      }
    }
  }
  
  if (idx == 32) {
    // Parse frame
    uint16_t pm25_raw = (buf[10] << 8) | buf[11];
    uint16_t pm10_raw = (buf[12] << 8) | buf[13];
    
    float pm25 = pm25_raw / 10.0;
    float pm10 = pm10_raw / 10.0;
    
    Serial.println("\n╔════════════════════╗");
    Serial.println("║ SDS011 - SUCCESS   ║");
    Serial.print("║ PM2.5: ");
    Serial.print(pm25, 1);
    Serial.println(" μg/m³      ║");
    Serial.print("║ PM10:  ");
    Serial.print(pm10, 1);
    Serial.println(" μg/m³      ║");
    Serial.println("╚════════════════════╝");
  } else {
    Serial.println("\n[ERROR] Failed to read SDS011 frame!");
    Serial.print("Bytes read: ");
    Serial.println(idx);
  }
}

// ===== DHT11 SENSOR =====
void test_dht11() {
  Serial.println("[DHT11] Testing Temperature/Humidity sensor...");
  
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  
  if (!isnan(t) && !isnan(h)) {
    Serial.println("╔════════════════════╗");
    Serial.println("║ DHT11 - SUCCESS    ║");
    Serial.print("║ Temp: ");
    Serial.print(t, 1);
    Serial.println("°C          ║");
    Serial.print("║ RH:   ");
    Serial.print(h, 1);
    Serial.println("%           ║");
    Serial.println("╚════════════════════╝");
  } else {
    Serial.println("[ERROR] DHT11 read failed!");
  }
}

// ===== MQ135 GAS SENSOR =====
void test_mq135() {
  Serial.println("[MQ135] Testing Gas/PPM sensor...");
  
  int raw = analogRead(MQ135_PIN);
  int ppm = map(raw, 0, 4095, 0, 500);
  float voltage = (raw / 4095.0) * 3.3;
  
  Serial.println("╔════════════════════╗");
  Serial.println("║ MQ135 - SUCCESS    ║");
  Serial.print("║ Raw:  ");
  Serial.print(raw);
  Serial.println("              ║");
  Serial.print("║ PPM:  ");
  Serial.print(ppm);
  Serial.println("              ║");
  Serial.print("║ Volt: ");
  Serial.print(voltage, 2);
  Serial.println("V           ║");
  Serial.println("╚════════════════════╝");
}

// ===== BME680 ENVIRONMENTAL SENSOR =====
void test_bme680() {
  Serial.println("[BME680] Testing Environmental sensor...");
  
  if (bme.performReading()) {
    Serial.println("╔════════════════════════════╗");
    Serial.println("║ BME680 - SUCCESS           ║");
    Serial.print("║ Temp: ");
    Serial.print(bme.temperature, 1);
    Serial.println("°C              ║");
    Serial.print("║ Press: ");
    Serial.print(bme.pressure / 100.0, 1);
    Serial.println(" hPa            ║");
    Serial.print("║ Alti:  ");
    Serial.print(bme.readAltitude(1013.25), 1);
    Serial.println(" m              ║");
    Serial.print("║ Gas:   ");
    Serial.print(bme.gas_resistance, 0);
    Serial.println(" Ω             ║");
    Serial.println("╚════════════════════════════╝");
  } else {
    Serial.println("[ERROR] BME680 read failed!");
  }
}

// ===== TEST ALL SENSORS =====
void test_all() {
  Serial.println("\n═══════════════════════════════════════");
  Serial.println("     TESTING ALL SENSORS");
  Serial.println("═══════════════════════════════════════\n");
  
  test_sds011();
  delay(500);
  
  test_dht11();
  delay(500);
  
  test_mq135();
  delay(500);
  
  test_bme680();
  
  Serial.println("\n═══════════════════════════════════════");
  Serial.println("     ALL TESTS COMPLETE");
  Serial.println("═══════════════════════════════════════\n");
}
