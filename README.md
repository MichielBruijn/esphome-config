# ESPHome configurations

My ESPHome configurations for Home Assistant. All passwords and API keys are stored in `secrets.yaml` (not in git).

## Setup

1. Copy `secrets.example.yaml` to `secrets.yaml`
2. Fill in your WiFi credentials, API key and OTA password
3. Flash via ESPHome dashboard or CLI

```bash
esphome run <device>.yaml
```

---

## Devices

### esp01s-rolluik-groot — Large roller shutter (relay, position-tracked)

**Hardware**
- Board: ESP-01S (ESP8266, 1MB flash)
- Relay module: 2-channel 5V relay (GPIO0 = up, GPIO2 = down, both inverted)

**What it does**
Controls a roller shutter with two relays. Position tracking is time-based (open/close duration configurable from Home Assistant). Supports:
- Open / close / stop
- Set position (0–100%)
- Non-blocking position movement via scripts
- Direction invert toggle in HA
- Safety interlock: both relays never active simultaneously

**Customisation**
- Set `open_time_number` and `close_time_number` in HA to the measured travel times of your motor.
- Set `slat_open_time_number` (ms) to the time it takes for the slats to rotate from fully closed to horizontal before the shutter starts rising. Default 3000 ms — increase if the shutter stops too low when moving from position 0, decrease if it overshoots.

---

### esp01s-rolluik-klein — Small roller shutter (relay, position-tracked)

Identical to `esp01s-rolluik-groot` but with slightly shorter default travel times (23s open, 21s close).

**Hardware**
- Board: ESP-01S (ESP8266, 1MB flash)
- Relay module: 2-channel 5V relay (GPIO0 / GPIO2)

---

### esp01s-zonnescherm — Awning / sun screen (relay, time_based)

**Hardware**
- Board: ESP-01S (ESP8266, 1MB flash)
- Relay module: 2-channel 5V relay (GPIO0 / GPIO2)

**What it does**
Controls an awning or sun screen using ESPHome's built-in `time_based` cover. Simpler than the roller shutter variant — no position tracking, just open/close/stop based on configured durations.

**Customisation**
Adjust `open_duration` and `close_duration` in the YAML to match your motor.

---

### esp32c3-airco-zolder-achter — Mitsubishi air conditioner attic rear (MHI SPI)

**Hardware**
- Board: ESP32-C3 DevKitM-1
- Connection: SPI directly on the CN105 connector of the Mitsubishi Heavy Industries (MHI) air conditioner
  - MOSI: GPIO6
  - MISO: GPIO5
  - SCLK: GPIO4
- Status LED: GPIO19 (white, off = OK)

**Software**
- External component: [hberntsen/mhi-ac-ctrl-esp32-c3](https://github.com/hberntsen/mhi-ac-ctrl-esp32-c3)
- Framework: ESP-IDF (required by this component)

**What it does**
Full Home Assistant integration of the air conditioner:
- Temperature control, mode, fan speed
- Vane direction (horizontal + vertical, requires `use_long_frame: true`)
- Energy monitoring (power, current, kWh)
- Diagnostic sensors: compressor frequency, pipe temperatures, run hours, etc.
- Defrost sensor

**Compatible with**
Mitsubishi Heavy Industries air conditioners with a CN105 connector. Tested on multi-split units.

---

### esp32c3-airco-zolder-voor — Mitsubishi air conditioner attic front (MHI SPI)

Identical to `esp32c3-airco-zolder-achter` but with a different device name/ID so both units appear separately in HA.

**Hardware / Software**
See `esp32c3-airco-zolder-achter` above.

---

### esp32c3-deurbel — Doorbell (relay pulse)

**Hardware**
- Board: ESP32-C3 DevKitM-1
- Relay: GPIO3 (inverted)
- Wiring: in series with or in parallel to the existing doorbell button circuit

**What it does**
Simulates a 500 ms button press. Trigger from an HA automation or directly via the switch entity. The relay turns off automatically after 500 ms.

**Usage**
- Turn on `Relay GPIO3` in HA → doorbell rings once
- Combine with a motion sensor or video doorbell for smart integrations

---

### esp32c3-power-meter — Smart meter reader (P1 / DSMR)

**Hardware**
- Board: ESP32-C3 DevKitM-1
- P1 cable: RJ11 to serial, signal inverted, 5V powered from the meter
  - RX: GPIO3 (inverted, pull-up, 7-bit EVEN parity, 115200 baud)

**What it does**
Reads the Dutch smart energy meter (DSMR P1 port) and sends data to Home Assistant:
- Electricity delivered / returned at low and high tariff (kWh)
- Current power (W)
- Gas consumption (m³)

**Customisation**
- DSMR v5 (most Dutch smart meters) works out of the box
- For older meters you may need to adjust `baud_rate` or parity settings

---

### esp32c3-rolluik-zolder-achter — 2× Somfy roller shutter attic (remote emulation)

**Hardware**
- Board: ESP32-C3 DevKitM-1
- Somfy remote 1 (up/stop/down): GPIO4 / GPIO5 / GPIO6 (open-drain, inverted)
- Somfy remote 2 (up/stop/down): GPIO1 / GPIO2 / GPIO3 (open-drain, inverted)
- Wiring: solder wires directly onto the button pads of two existing Somfy remote controls

**What it does**
Controls two Somfy roller shutters by emulating the button presses of two existing remote controls. Position tracking is time-based.

Supports:
- Open / close / stop for each shutter independently
- Set position (0–100%)
- Ventilation position: shortened close duration when closing from ventilation position
- FORCE buttons to send direct pulses to the remote (bypasses position logic)
- Non-blocking scripts — both shutters operable simultaneously

**Customisation**
- Set open/close duration per shutter in HA via `Somfy 1/2 open time` and `close time`
- Set `vent_pos` (%) and `vent_close_ms` (ms) for your ventilation position
- Set `Somfy 1/2 slat open time (ms)` to the time it takes for the slats to rotate from fully closed to horizontal before the shutter starts rising. Default 3000 ms — tune upward if position accuracy is off when opening from fully closed.

---

### esp32c3-tft-watermeter — Water meter (pulse counter + OLED display)

**Hardware**
- Board: ESP32-C3 DevKitM-1
- Pulse input: GPIO4 (pull-up, inverted) — connect to reed switch or Hall sensor on water meter
- OLED display: SSD1306 72×40 px via I2C (SDA=GPIO5, SCL=GPIO6, address 0x3C)
- Boot button: GPIO9 (built-in on DevKit) for manual correction

**What it does**
Counts water consumption via pulses (1 pulse = 1 litre). Sends to Home Assistant:
- Current flow rate (L/min)
- Total meter reading (m³, 3 decimal places)

The OLED display shows the meter reading locally, split into integer and decimal parts.

The on-board boot button acts as a manual correction:
- Short press (< 350 ms): +1 litre
- Long press (800 ms–3 s): −1 litre

To set the starting value, write the correct m³ value to `Water meter correction` in HA.

**Customisation**
- Verify your meter outputs 1 pulse per litre (most Dutch water meters do)
- Adjust OLED type and I2C address if needed

---

### esp32c3-zout-niveau — Water softener salt level (VL53L1X ToF)

**Hardware**
- Board: ESP32-C3 DevKitM-1
- Sensor: VL53L1X Time-of-Flight distance sensor (I2C, SDA=GPIO4, SCL=GPIO3, address 0x29)
- Mounting: sensor mounted at the top of the salt tank pointing downward

**Software**
- Library: [pololu/VL53L1X](https://github.com/pololu/vl53l1x-arduino) via PlatformIO
- Custom helper: `vl53l1x_helper.h` (included in this repo)

**What it does**
Measures the distance to the salt surface and converts it to a percentage (0% = empty, 100% = full). Calibration values are configurable from HA:
- `Salt distance FULL (cm)`: distance in cm when tank is full (e.g. 2.5 cm)
- `Salt distance EMPTY (cm)`: distance in cm when tank is empty (e.g. 34 cm)

Measurement interval configurable (5–300 seconds). "Measure now" button available for an on-demand reading.

**Customisation**
- Calibrate `depth_full_cm` and `depth_empty_cm` for your tank via the HA interface
- VL53L1X `Short` mode works up to ~130 cm; switch to `Medium` or `Long` for larger tanks
