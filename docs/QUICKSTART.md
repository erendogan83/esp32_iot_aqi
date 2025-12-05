# Quick Start Guide

## 5-Minute Setup

### 1. Hardware (10 min)

Wire sensors according to `hardware/schematic.md`:
- SDS011 → UART1 (GPIO16/17)
- DHT11 → GPIO5
- MQ135 → GPIO34 (ADC)
- BME680 → I2C (GPIO21/22)

### 2. Configuration (5 min)

Edit `firmware/include/config.h`:
```cpp
#define WIFI_SSID "your_network"
#define WIFI_PASSWORD "password"
#define MQTT_SERVER "mqtt.example.com"
#define STATION_ID "STATION_001"
```

### 3. Upload (5 min)

- Open `firmware/src/main.cpp` in Arduino IDE
- Select **ESP32 WROOM** board
- Select correct COM port
- Click **Upload**

### 4. Verify (5 min)

Open Serial Monitor (115200 baud):
- Should show WiFi connection
- Should show MQTT connection
- Sensor readings displayed

## Troubleshooting

- **No WiFi**: Check SSID and password
- **BME680 init fails**: Check I2C address (0x77 vs 0x76)
- **No PM readings**: Check SDS011 RX/TX wiring
- **DHT errors**: Wait for device warm-up

## Next Steps

1. Set up MQTT broker (see `cloud/`)
2. Configure InfluxDB
3. Create Grafana dashboard
4. Deploy to production
