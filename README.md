# 🌍 ESP32 IoT AQI - Air Quality Index Monitoring System

![Status](https://img.shields.io/badge/Status-Production%20Ready-brightgreen?style=flat-square)
![Version](https://img.shields.io/badge/Version-1.0.0-blue?style=flat-square)
![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)
![Platform](https://img.shields.io/badge/Platform-ESP32%20WROOM-orange?style=flat-square)

> **Real-time air quality monitoring system** using ESP32 microcontroller with multiple environmental sensors and cloud integration (MQTT → InfluxDB → Grafana)

---

## 📋 Table of Contents

- [✨ Features](#-features)
- [🔌 Hardware Components](#-hardware-components)
- [☁️ Cloud Architecture](#️-cloud-architecture)
- [🚀 Quick Start](#-quick-start)
- [📊 Sensor Details](#-sensor-details)
- [💾 Project Structure](#-project-structure)
- [⚙️ Configuration](#️-configuration)
- [📡 Data Format](#-data-format)
- [🔧 Troubleshooting](#-troubleshooting)
- [📜 License](#-license)

---

## ✨ Features

| Feature | Details |
|---------|---------|
| 📊 **Real-time Monitoring** | PM2.5, PM10, Temperature, Humidity, Pressure, Gas, Altitude |
| 🌐 **WiFi Connectivity** | Built-in ESP32 WiFi (802.11 b/g/n) |
| 📡 **MQTT Protocol** | Mosquitto, AWS IoT, Azure IoT Hub support |
| 📈 **Cloud Integration** | InfluxDB time-series database |
| 📉 **Visualization** | Grafana dashboards included |
| ⚙️ **Configurable** | Measurement intervals, calibration, thresholds |
| 💰 **Cost-Effective** | ~$100-150 per monitoring station |
| 🔓 **Open Source** | MIT License, full source code |

---

## 🔌 Hardware Components

| Component | Model | Parameter | Interface | Power | Cost |
|-----------|-------|-----------|-----------|-------|------|
| 📊 **PM Sensor** | **SDS011** | PM2.5, PM10 | UART 9600 | 5V/100mA | $25 |
| 🌡️ **Temp/Humidity** | **DHT11** | T (0-50°C), RH (20-80%) | Digital GPIO5 | 3.3V/5mA | $4 |
| 💨 **Gas Sensor** | **MQ135** | PPM, VOC Detection | Analog GPIO34 | 5V/200mA | $10 |
| 🏔️ **Environmental** | **BME680** | Pressure, Altitude, Gas | I2C GPIO21/22 | 3.3V/12mA | $15 |
| 🎛️ **Microcontroller** | **ESP32 WROOM** | Dual-core 240MHz, WiFi, BLE | SPI/UART/I2C | 5V/200mA | $12 |
| | | | | **TOTAL** | **$90-120** |

---

## ☁️ Cloud Architecture

```
┌──────────────────┐
│   Sensors        │
│ • SDS011 (PM)    │
│ • DHT11 (T/H)    │
│ • MQ135 (Gas)    │
│ • BME680 (Env)   │
└────────┬─────────┘
         │
    ┌────▼────┐
    │  ESP32   │  ← Main processing unit
    │  WROOM   │     (WiFi + MQTT)
    └────┬────┘
         │
    ┌────▼──────────────┐
    │   WiFi Network    │
    └────┬──────────────┘
         │
    ┌────▼──────────────┐
    │  MQTT Broker      │  ← Mosquitto/AWS/Azure
    │  (aqi/+/data)     │
    └────┬──────────────┘
         │
    ┌────▼──────────────┐
    │   InfluxDB        │  ← Time-Series Database
    │  (Time-Series)    │
    └────┬──────────────┘
         │
    ┌────▼──────────────┐
    │   Grafana         │  ← Visualization & Dashboards
    │  (Dashboards)     │
    └───────────────────┘
```

---

## 🚀 Quick Start

### ⏱️ Setup Time: 30 minutes

#### Step 1️⃣: Hardware Assembly (15 min)

Wire sensors to ESP32 WROOM according to `hardware/schematic.md`:

```
ESP32 PIN          SENSOR              SIGNAL
─────────────────────────────────────────────
GPIO16/17          SDS011              UART1
GPIO5              DHT11               Digital
GPIO34             MQ135               Analog
GPIO21/22          BME680              I2C (0x77)
5V                 SDS011, MQ135       Power
3.3V               DHT11, BME680       Power
GND                All                 Ground
```

#### Step 2️⃣: Configure (5 min)

Edit `firmware/include/config.h`:

```cpp
#define WIFI_SSID "your_ssid"
#define WIFI_PASSWORD "your_password"
#define MQTT_SERVER "mqtt.example.com"
#define MQTT_PORT 1883
#define MQTT_USER "user"
#define MQTT_PASSWORD "pass"
#define INFLUXDB_HOST "influxdb.example.com"
#define STATION_ID "STATION_001"
```

#### Step 3️⃣: Upload (5 min)

1. Open Arduino IDE
2. Select **ESP32 WROOM** board
3. Open `firmware/src/main.cpp`
4. Click **Upload**

#### Step 4️⃣: Verify (5 min)

Open Serial Monitor (115200 baud):
```
=== ESP32 IoT AQI v1.0 ===
[WiFi] Connected! IP: 192.168.1.100
[MQTT] Connected
[SDS011] PM2.5: 42.3 μg/m³, PM10: 68.5 μg/m³
[DHT11] T: 22.5°C, H: 65%
[BME680] Pressure: 1013.25 hPa, Altitude: 145m
[PUBLISH] Success!
```

---

## 📊 Sensor Details

### 📊 SDS011 - Particulate Matter Sensor
- **Measurement**: PM2.5, PM10 (μg/m³)
- **Range**: 0-999 μg/m³
- **Accuracy**: ±10%
- **Interface**: UART (9600 baud)
- **Frame Format**: `[0x42][0x4D][...][CRC][0x0D][0x0A]`

### 🌡️ DHT11 - Temperature & Humidity
- **Temperature**: 0-50°C (±2°C accuracy)
- **Humidity**: 20-80% RH (±5% accuracy)
- **Read Interval**: Minimum 2 seconds
- **Interface**: Single digital pin

### 💨 MQ135 - Gas/VOC Sensor
- **Detects**: CO2, Ammonia, Benzene, VOCs
- **Output**: 0-3.3V analog signal
- **Range**: 0-500 PPM (configurable)
- **Warm-up Time**: 20 minutes (first use)

### 🏔️ BME680 - Environmental Sensor
- **Temperature**: -40 to +85°C (±1°C)
- **Pressure**: 300-1100 hPa (±1 hPa)
- **Humidity**: 0-100% RH (±3%)
- **Gas Resistance**: 0-4000+ kΩ
- **Interface**: I2C (400 kHz) @ 0x77 or 0x76

---

## 💾 Project Structure

```
esp32-iot-aqi/
│
├── 📁 firmware/
│   ├── src/
│   │   └── main.cpp              ← ESP32 Main Code (750+ lines)
│   └── include/
│       └── config.h              ← Configuration Header
│
├── 📁 cloud/
│   ├── mqtt-setup.md             ← MQTT Broker Setup
│   ├── influxdb-setup.md         ← InfluxDB Installation
│   └── grafana-dashboard.json    ← Grafana Template
│
├── 📁 examples/
│   └── sensor_test.ino           ← Individual Sensor Tests
│
├── 📁 hardware/
│   ├── schematic.md              ← Wiring Diagram
│   └── bom.csv                   ← Bill of Materials
│
├── 📁 docs/
│   ├── QUICKSTART.md             ← 5-Minute Setup
│   ├── TROUBLESHOOTING.md        ← Common Issues
│   └── API.md                    ← Data Format
│
├── README.md                      ← This File
├── LICENSE                        ← MIT License
└── .gitignore                     ← Git Configuration
```

---

## ⚙️ Configuration

All settings in **`firmware/include/config.h`**:

| Setting | Default | Purpose |
|---------|---------|---------|
| `WIFI_SSID` | your_ssid | WiFi network name |
| `WIFI_PASSWORD` | your_password | WiFi password |
| `MQTT_SERVER` | mqtt.example.com | MQTT broker address |
| `MQTT_PORT` | 1883 | MQTT port (1883 standard) |
| `STATION_ID` | STATION_001 | Unique station identifier |
| `MEASUREMENT_INTERVAL` | 900 | Measurement interval (seconds) |
| `INFLUXDB_HOST` | influxdb.example.com | InfluxDB server |

---

## 📡 Data Format

### MQTT Topic
```
aqi/STATION_001/data
```

### JSON Payload
```json
{
  "station_id": "STATION_001",
  "station_name": "My Air Quality Station",
  "timestamp": "2025-12-05T14:30:00Z",
  "uptime_sec": 86400,
  
  "measurements": {
    "pm25": 42.3,              ← SDS011 (μg/m³)
    "pm10": 68.5,              ← SDS011 (μg/m³)
    "temperature": 22.5,       ← DHT11 (°C)
    "humidity": 65,            ← DHT11 (%)
    "pressure": 1013.25,       ← BME680 (hPa)
    "altitude": 145,           ← BME680 (m)
    "gas_resistance": 125000,  ← BME680 (Ω)
    "ppm": 285                 ← MQ135 (PPM)
  },
  
  "quality": {
    "flag": 0,                 ← 0=valid, >0=error
    "wifi_rssi": -65           ← WiFi signal (dBm)
  }
}
```

---

## 🔧 Troubleshooting

| Issue | Cause | Solution |
|-------|-------|----------|
| ❌ No WiFi connection | Wrong SSID/password | Check `config.h` WiFi settings |
| ❌ MQTT connection fails | Broker unreachable | Verify MQTT_SERVER address |
| ❌ SDS011 not reading | UART wiring reversed | Check GPIO16/17 connections |
| ❌ DHT11 errors | Timing issue | Wait 2+ seconds between reads |
| ❌ BME680 init fails | Wrong I2C address | Try 0x76 if 0x77 fails |
| ❌ No data in InfluxDB | Telegraf not running | Start MQTT→InfluxDB bridge |

---

## 📦 Dependencies

### Arduino Libraries
- `PubSubClient` (MQTT)
- `DHT sensor library` (DHT11)
- `Adafruit_BME680` (BME680)
- `ArduinoJson` (JSON processing)

### Cloud Services
- MQTT Broker (Mosquitto/AWS/Azure)
- InfluxDB (time-series database)
- Grafana (visualization)

---

## 📝 Example Usage

### Test Individual Sensors
```bash
# Upload examples/sensor_test.ino
# Serial Monitor → Type:
# 1 = SDS011
# 2 = DHT11
# 3 = MQ135
# 4 = BME680
# 5 = ALL
```

### Subscribe to MQTT
```bash
mosquitto_sub -h mqtt.broker.com \
  -u mqtt_user \
  -P mqtt_pass \
  -t "aqi/STATION_001/data"
```

### Query InfluxDB
```sql
SELECT * FROM aqi_data 
WHERE station_id = 'STATION_001' 
  AND time > now() - 24h
```

---

## 📊 Performance Specs

| Metric | Value |
|--------|-------|
| Measurement Interval | 5-15 minutes (configurable) |
| WiFi Range | ~50-100 meters |
| WiFi Signal | -40 to -80 dBm typical |
| MQTT Message Size | ~200 bytes |
| System Uptime | 30+ days (continuous operation) |
| Power Consumption | ~350mA average |
| Boot Time | <10 seconds |

---

## 👨‍💻 Author

**Created by**: Eren DOGAN
**Project**: ESP32 IoT Air Quality Monitoring System  
**Started**: December 2025  

---

## 📄 License

MIT License - See `LICENSE` file for details

```
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software.
```

---

## 🤝 Contributing

Contributions welcome! Please:
- Test code on ESP32 hardware
- Document sensor changes
- Include calibration details
- Follow existing code style

---

## 🙏 Acknowledgments

- Espressif Systems (ESP32)
- Plantower (SDS011 sensor)
- Bosch Sensortec (BME680)
- Arduino Community
- Open Source Contributors

---

## 📞 Support

- 📚 **Documentation**: See `docs/` folder
- 🐛 **Issues**: Create GitHub issue
- 💬 **Questions**: Check `docs/TROUBLESHOOTING.md`
- 📧 **Contact**: erendogann83@gmail.com

---

**Last Updated**: December 5, 2025  
**Version**: 1.0.0  
**Status**: ✅ Production Ready

---

*This project is maintained with ❤️ by Eren DOGAN
