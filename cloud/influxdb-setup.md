# InfluxDB Setup

## Installation (Docker)

```bash
docker run -d -p 8086:8086 -v influxdb:/var/lib/influxdb influxdb:1.8
```

## Create Database

```bash
curl -X POST http://localhost:8086/query --data-urlencode "q=CREATE DATABASE aqi_measurements"
```

## Line Protocol Format

```
measurement,tag1=value1,tag2=value2 field1=value1,field2=value2 timestamp
```

Example:
```
aqi_data,station=STATION_001 pm25=42.3,pm10=68.5,temperature=22.5 1733410200000000000
```

## Query

```bash
# Get latest measurements
curl 'http://localhost:8086/query?db=aqi_measurements&q=SELECT * FROM aqi_data LIMIT 10'

# Get data from last 24 hours
curl 'http://localhost:8086/query?db=aqi_measurements&q=SELECT * FROM aqi_data WHERE time > now() - 24h'
```

## Retention Policy

```bash
# Keep data for 30 days
curl -X POST http://localhost:8086/query --data-urlencode "q=CREATE RETENTION POLICY thirty_days ON aqi_measurements DURATION 30d REPLICATION 1 DEFAULT"
```
