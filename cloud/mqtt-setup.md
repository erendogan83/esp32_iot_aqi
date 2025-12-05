# MQTT Broker Setup

## Using Mosquitto (Self-hosted)

### Install
```bash
sudo apt-get install mosquitto mosquitto-clients
sudo systemctl start mosquitto
sudo systemctl enable mosquitto
```

### Configure /etc/mosquitto/mosquitto.conf
```
listener 1883
protocol mqtt

# Optional: Username/Password
#allow_anonymous false
#password_file /etc/mosquitto/passwd
```

### Create User
```bash
sudo mosquitto_passwd -c /etc/mosquitto/passwd mqtt_user
# Enter password when prompted
```

### Test
```bash
mosquitto_sub -h localhost -t "aqi/+/data"
```

## Using AWS IoT Core

1. Create thing, certificates, and policy
2. Download certificates
3. Update MQTT_SERVER, MQTT_USER in config.h
4. Use certificates for authentication

## Using Azure IoT Hub

1. Create IoT Hub and device
2. Get connection string
3. Use device-specific endpoint in config.h

## Publishing Test

```bash
mosquitto_pub -h mqtt.example.com -t "aqi/STATION_001/data" \
  -m '{"station_id":"STATION_001","pm25":42.3}'
```

## Topics

```
aqi/STATION_001/data    → Measurements (from device)
aqi/STATION_001/status  → Status (online/offline)
aqi/STATION_001/config  → Configuration (to device)
```
