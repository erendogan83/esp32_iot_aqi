# Hardware Schematic - ESP32 IoT AQI

## Wiring Diagram

```
┌────────────────────────────────────────────┐
│         ESP32 WROOM Microcontroller        │
├────────────────────────────────────────────┤
│                                            │
│  SDS011 (PM Sensor - UART1):              │
│  ├─ TX  → GPIO16 (RX pin)                 │
│  ├─ RX  → GPIO17 (TX pin)                 │
│  ├─ GND → GND                             │
│  └─ 5V  → 5V                              │
│                                            │
│  DHT11 (Temperature/Humidity):            │
│  ├─ Signal → GPIO5                        │
│  ├─ GND    → GND                          │
│  └─ VCC    → 3.3V                         │
│                                            │
│  MQ135 (Gas Sensor - Analog):             │
│  ├─ AOUT → GPIO34 (ADC)                   │
│  ├─ GND  → GND                            │
│  └─ VCC  → 5V                             │
│                                            │
│  BME680 (Environmental - I2C):            │
│  ├─ SDA → GPIO21 (I2C SDA)                │
│  ├─ SCL → GPIO22 (I2C SCL)                │
│  ├─ GND → GND                             │
│  ├─ VCC → 3.3V                            │
│  └─ CSB → 3.3V (I2C mode)                 │
│                                            │
└────────────────────────────────────────────┘
```

## Pin Configuration Summary

| ESP32 GPIO | Sensor | Signal | Function |
|-----------|--------|--------|----------|
| GPIO16 | SDS011 | UART1 RX | Receive PM data |
| GPIO17 | SDS011 | UART1 TX | Send commands |
| GPIO5 | DHT11 | Digital | Temperature/Humidity |
| GPIO34 | MQ135 | Analog (ADC) | Gas level |
| GPIO21 | BME680 | I2C SDA | I2C data |
| GPIO22 | BME680 | I2C SCL | I2C clock |
| GND | All | Ground | Common return |
| 5V | SDS011, MQ135 | Power | ~400mA max |
| 3.3V | DHT11, BME680 | Power | ~50mA max |

## Power Budget

| Component | Voltage | Current | Power |
|-----------|---------|---------|-------|
| ESP32 | 5V | 80-200mA | 0.4-1.0W |
| SDS011 | 5V | 95-120mA | 0.5-0.6W |
| MQ135 | 5V | 150-200mA | 0.75-1.0W |
| DHT11 | 3.3V | 1-5mA | 0.003-0.015W |
| BME680 | 3.3V | 3-12mA | 0.01-0.04W |
| **Total** | | **~350-500mA** | **~1.7-2.6W** |

**Recommendation**: Use 5V/2A power supply (USB or DC adapter)

## I2C Address Detection

BME680 has configurable I2C address:
- **0x77** (default): Connect CSB to 3.3V
- **0x76**: Connect CSB to GND

Use I2C scanner to verify address if initialization fails.

## UART Configuration

**SDS011 Serial Settings**:
- Baud Rate: 9600
- Data Bits: 8
- Parity: None
- Stop Bits: 1
- Flow Control: None

Frame format: [0x42][0x4d][...data...][CRC][0x0d][0x0a]

## Sensor Specifications

### SDS011 - Particulate Matter Sensor
- Particles: PM2.5, PM10
- Range: 0-999 μg/m³
- Accuracy: ±10% (0-500 μg/m³)
- Output: UART 9600 baud
- Warm-up: 30-60 seconds
- Current: 95-120mA

### DHT11 - Temperature/Humidity
- Temperature: 0-50°C (±2°C)
- Humidity: 20-80% RH (±5%)
- Output: Single digital signal
- Read Interval: Min 2 seconds
- Current: 2.5mA average

### MQ135 - Gas Sensor
- Detects: CO2, Ammonia, Benzene, VOCs
- Output: Analog 0-5V
- Response Time: <10s
- Operating Temp: -10 to 50°C
- Current: 150-200mA

### BME680 - Environmental Sensor
- Temperature: -40 to +85°C (±1°C)
- Pressure: 300 to 1100 hPa (±1 hPa)
- Humidity: 0-100% RH (±3%)
- Gas Resistance: 0 to 4000+ kΩ
- Interface: I2C (400 kHz)
- Address: 0x76 or 0x77
- Current: 3-12mA

## PCB Considerations

1. **Power Rails**:
   - Separate 5V line for SDS011 and MQ135
   - Separate 3.3V line for DHT11 and BME680
   - Use voltage regulator (AMS1117 for 3.3V)

2. **Decoupling**:
   - 100μF capacitor near 5V supply
   - 10μF capacitor near 3.3V supply
   - 0.1μF capacitor near each IC

3. **Ground**:
   - Star point grounding
   - Separate digital/analog grounds if needed
   - Connect to ESP32 GND pins

4. **Trace Layout**:
   - Keep UART traces short (SDS011)
   - I2C pull-ups: 4.7kΩ to 3.3V
   - Ferrite bead on MQ135 power line

5. **Routing**:
   - Signal traces: 0.3mm minimum
   - Power traces: 0.6mm minimum
   - Via size: 0.3mm

## Testing Checklist

- [ ] Verify 5V supply at SDS011 connector
- [ ] Verify 3.3V supply at DHT11 and BME680
- [ ] Check I2C communication (0x77 BME680)
- [ ] Check UART communication (9600 baud SDS011)
- [ ] Verify GPIO pins don't have conflicts
- [ ] Test each sensor individually
- [ ] Monitor power consumption

## Troubleshooting

**SDS011 not responding**:
- Check TX/RX wired correctly (not crossed)
- Verify baud rate is 9600
- Check for 5V power

**DHT11 reading errors**:
- Check GPIO5 pull-up resistor
- Verify 3.3V power
- Wait 2 seconds between reads

**BME680 init fails**:
- Check I2C address (0x77 vs 0x76)
- Verify 3.3V supply
- Check pull-ups on SDA/SCL

**MQ135 no readings**:
- Check ADC pin assignment
- Verify 5V power
- Wait for warm-up (20 minutes first use)

## Cost Estimate

| Item | Supplier | Price |
|------|----------|-------|
| ESP32 WROOM | AliExpress | $10-15 |
| SDS011 | AliExpress | $20-25 |
| DHT11 | Amazon | $3-5 |
| MQ135 | AliExpress | $8-12 |
| BME680 | Adafruit/Amazon | $15-20 |
| PCB + Assembly | Custom | $10-15 |
| Cables/Connectors | Local | $5-10 |
| Housing/Enclosure | Local | $10-15 |
| **Total** | | **$90-120** |
