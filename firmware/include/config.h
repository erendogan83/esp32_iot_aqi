/*
 * ESP32 IoT AQI - Configuration Header
 * Sensors: SDS011 + DHT11 + MQ135 + BME680
 * Transmission: MQTT + InfluxDB
 */

#ifndef CONFIG_H
#define CONFIG_H

// ===== WIFI CONFIGURATION =====
#define WIFI_SSID "your_ssid"
#define WIFI_PASSWORD "your_password"
#define WIFI_TIMEOUT 20000  // milliseconds

// ===== MQTT BROKER =====
#define MQTT_SERVER "mqtt.example.com"
#define MQTT_PORT 1883
#define MQTT_USER "mqtt_user"
#define MQTT_PASSWORD "mqtt_pass"

// ===== MQTT TOPICS =====
#define MQTT_TOPIC_DATA "aqi/STATION_001/data"
#define MQTT_TOPIC_STATUS "aqi/STATION_001/status"
#define MQTT_TOPIC_CONFIG "aqi/STATION_001/config"

// ===== STATION IDENTIFICATION =====
#define STATION_ID "STATION_001"
#define STATION_NAME "My Air Quality Station"
#define LOCATION_LAT 40.1895
#define LOCATION_LON 29.0244

// ===== MEASUREMENT SETTINGS =====
#define MEASUREMENT_INTERVAL 900  // 15 minutes in seconds
#define SERIAL_BAUD 115200

// ===== INFLUXDB CONFIGURATION (Optional) =====
#define INFLUXDB_ENABLED true
#define INFLUXDB_HOST "influxdb.example.com"
#define INFLUXDB_PORT 8086
#define INFLUXDB_DB "aqi_measurements"
#define INFLUXDB_USER "influx_user"
#define INFLUXDB_PASS "influx_pass"

// ===== SENSOR PIN ASSIGNMENTS =====
// SDS011 Particulate Sensor (UART1)
#define SDS_RX_PIN 16  // GPIO16 - UART1 RX
#define SDS_TX_PIN 17  // GPIO17 - UART1 TX

// DHT11 Temperature/Humidity
#define DHT_PIN 5      // GPIO5 - Digital input

// MQ135 Gas Sensor
#define MQ135_PIN 34   // GPIO34 - ADC (analog input)

// BME680 Environmental Sensor (I2C)
#define BME680_SDA 21  // GPIO21 - I2C SDA
#define BME680_SCL 22  // GPIO22 - I2C SCL
#define BME680_ADDR 0x77  // I2C address (0x77 or 0x76)

// ===== SENSOR CALIBRATION =====
#define DHT_TEMP_OFFSET 0.0    // Temperature offset in °C
#define DHT_HUMIDITY_OFFSET 0  // Humidity offset in %
#define MQ135_CALIBRATION 1.0  // Calibration multiplier
#define SDS_PM_OFFSET 0.0      // PM offset

// ===== FEATURES =====
#define DEBUG_SERIAL true      // Enable serial debug output
#define ENABLE_MQTT true       // Enable MQTT transmission
#define ENABLE_INFLUXDB true   // Enable InfluxDB integration
#define ENABLE_DEEP_SLEEP false // Enable deep sleep mode

// ===== THRESHOLDS FOR ALERTS =====
#define PM25_ALERT_THRESHOLD 150.0  // μg/m³
#define PM10_ALERT_THRESHOLD 254.0  // μg/m³

#endif // CONFIG_H
