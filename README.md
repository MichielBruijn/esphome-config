# ESPHome configuraties

Mijn ESPHome configuraties voor Home Assistant. Alle wachtwoorden en API-keys zitten in `secrets.yaml` (niet in git).

## Gebruik

1. Kopieer `secrets.example.yaml` naar `secrets.yaml`
2. Vul je eigen WiFi, API key en OTA password in
3. Flash via ESPHome dashboard of CLI

```bash
esphome run <device>.yaml
```

---

## Devices

### esp01s-rolluik-groot — Groot rolluik (relay, positiegestuurd)

**Hardware**
- Board: ESP-01S (ESP8266, 1MB flash)
- Relaismodule: 2-kanaals 5V relay (GPIO0 = omhoog, GPIO2 = omlaag, beide geïnverteerd)

**Wat doet het**
Bestuur een rolluik met twee relais. Positiebepaling gebeurt op basis van tijd (open/sluitduur instelbaar in Home Assistant). Ondersteunt:
- Open / dicht / stop
- Positie instellen (0–100 %)
- Non-blocking positiebeweging via scripts
- Richting omkeren (toggle in HA)
- Veilige interlock: beide relais nooit tegelijk aan

**Aanpassen**
- `open_time_number` en `close_time_number` in HA instellen op gemeten tijden van jouw motor

---

### esp01s-rolluik-klein — Klein rolluik (relay, positiegestuurd)

Identiek aan `esp01s-rolluik-groot`, maar standaard iets kortere rijdtijden (23s open, 21s sluit).

**Hardware**
- Board: ESP-01S (ESP8266, 1MB flash)
- Relaismodule: 2-kanaals 5V relay (GPIO0 / GPIO2)

---

### esp01s-zonnescherm — Zonnescherm (relay, time_based)

**Hardware**
- Board: ESP-01S (ESP8266, 1MB flash)
- Relaismodule: 2-kanaals 5V relay (GPIO0 / GPIO2)

**Wat doet het**
Bestuur een zonnescherm/markies via ESPHome's ingebouwde `time_based` cover. Eenvoudiger dan de rolluik-variant: geen positiebewaking, alleen open/dicht/stop op basis van ingestelde tijden.

**Aanpassen**
- `open_duration` en `close_duration` in de YAML aanpassen op jouw motor

---

### esp32c3-airco-zolder-achter — Mitsubishi airco zolder achter (MHI SPI)

**Hardware**
- Board: ESP32-C3 DevKitM-1
- Aansluiting: SPI direct op de CN105-connector van de Mitsubishi Heavy Industries (MHI) airconditioner
  - MOSI: GPIO6
  - MISO: GPIO5
  - SCLK: GPIO4
- Status-LED: GPIO19 (wit, uit = OK)

**Software**
- Externe component: [hberntsen/mhi-ac-ctrl-esp32-c3](https://github.com/hberntsen/mhi-ac-ctrl-esp32-c3)
- Framework: ESP-IDF (vereist voor deze component)

**Wat doet het**
Volledige Home Assistant integratie van de airco:
- Temperatuurregeling, stand, ventilatorsnelheid
- Lamellen richting (horizontaal + verticaal, vereist `use_long_frame: true`)
- Energiemeting (vermogen, stroom, kWh)
- Diagnose-sensoren: compressorfrequentie, buistemperaturen, loopuren, etc.
- Ontdooiingssensor

**Werkt met**
Mitsubishi Heavy Industries airconditioners met CN105-aansluiting. Getest op multi-split units.

---

### esp32c3-airco-zolder-voor — Mitsubishi airco zolder voor (MHI SPI)

Identiek aan `esp32c3-airco-zolder-achter`, andere device-naam/ID zodat beide in HA naast elkaar bestaan.

**Hardware / Software**
Zie `esp32c3-airco-zolder-achter` hierboven.

---

### esp32c3-deurbel — Deurbel (relay puls)

**Hardware**
- Board: ESP32-C3 DevKitM-1
- Relay: GPIO3 (geïnverteerd)
- Aansluiting: in serie met of parallel aan bestaand drukknop-circuit

**Wat doet het**
Simuleert een drukknop-puls van 500ms. Activeer via HA-automatisering of direct via de schakelaar. Na 500ms gaat het relais automatisch weer uit.

**Gebruik**
- Schakel `Relay GPIO3` aan in HA → deurbel geeft één keer signaal
- Combineer met een bewegingssensor of video-deurbel voor een slimme koppeling

---

### esp32c3-power-meter — Slimme meter uitlezer (P1/DSMR)

**Hardware**
- Board: ESP32-C3 DevKitM-1
- P1-kabel: RJ11 naar serieel, signaal inverted, 5V voeding vanuit meter
  - RX: GPIO3 (inverted, pull-up, 7-bit EVEN parity, 115200 baud)

**Wat doet het**
Leest de Nederlandse slimme energiemeter (DSMR P1-poort) uit en stuurt de gegevens naar Home Assistant:
- Stroomverbruik/teruglevering laag- en hoogtarief (kWh)
- Actueel vermogen (W)
- Gasverbruik (m³)

**Aanpassen**
- DSMR v5 (meeste slimme meters) werkt out-of-the-box
- Bij oudere meters eventueel `baud_rate` of pariteit aanpassen

---

### esp32c3-rolluik-zolder-achter — 2x Somfy rolluik zolder (remote emulatie)

**Hardware**
- Board: ESP32-C3 DevKitM-1
- Somfy remote 1 (open/stop/dicht): GPIO4 / GPIO5 / GPIO6 (open-drain, inverted)
- Somfy remote 2 (open/stop/dicht): GPIO1 / GPIO2 / GPIO3 (open-drain, inverted)
- Aansluiting: direct op de knoppen van de Somfy-afstandsbediening (soldeer draden op de knop-pads)

**Wat doet het**
Bestuur twee Somfy-rolluiken door de knoppen van twee bestaande afstandsbedieningen te simuleren. Positiebepaling op basis van tijd.

Ondersteunt:
- Open / dicht / stop voor beide rolluiken onafhankelijk
- Positie instellen (0–100 %)
- Ventilatie-positie: bij sluiten vanuit ventilatiepositie kortere sluitduur
- FORCE-knoppen om de remote direct aan te sturen (bypass van positielogica)
- Non-blocking scripts, beide rolluiken tegelijk bedienbaar

**Aanpassen**
- Open/sluitduur per rolluik instellen in HA via `Somfy 1/2 opentijd` en `sluitijd`
- `vent_pos` (%) en `vent_close_ms` (ms) instellen voor jouw ventilatiepositie

---

### esp32c3-tft-watermeter — Watermeter (puls + OLED display)

**Hardware**
- Board: ESP32-C3 DevKitM-1
- Puls-ingang: GPIO4 (pull-up, inverted) — aansluiten op reed-contact of Hall-sensor van watermeter
- OLED display: SSD1306 72×40px via I2C (SDA=GPIO5, SCL=GPIO6, adres 0x3C)
- Boot-knop: GPIO9 (ingebouwd op DevKit) voor handmatige correctie

**Wat doet het**
Telt waterverbruik via pulsen (1 puls = 1 liter). Stuurt naar Home Assistant:
- Actuele doorstroming (L/min)
- Totale meterstand (m³, 3 decimalen)

Het OLED-display toont de meterstand lokaal in twee gedeelten (geheel / decimalen).

De boot-knop op het bordje dient als handmatige correctie:
- Kort klikken (< 350ms): +1 liter
- Lang indrukken (800ms–3s): −1 liter

Correctie van de beginstand via HA: schrijf de correcte m³-waarde naar `Watermeter correctie`.

**Aanpassen**
- Controleer of jouw meter 1 puls/liter geeft (meeste Nederlandse watermeters doen dit)
- OLED-type en I2C-adres eventueel aanpassen

---

### esp32c3-zout-niveau — Zoutniveau waterontharder (VL53L1X ToF)

**Hardware**
- Board: ESP32-C3 DevKitM-1
- Sensor: VL53L1X Time-of-Flight afstandssensor (I2C, SDA=GPIO4, SCL=GPIO3, adres 0x29)
- Bevestiging: sensor van boven in de zoutbak van de waterontharder gericht naar beneden

**Software**
- Library: [pololu/VL53L1X](https://github.com/pololu/vl53l1x-arduino) via PlatformIO
- Custom helper: `vl53l1x_helper.h` (zit in deze repo)

**Wat doet het**
Meet de afstand tot het zoutoppervlak en rekent dit om naar een percentage (0% = leeg, 100% = vol). Kalibratiewaardes instelbaar vanuit HA:
- `Zout afstand VOL (cm)`: afstand in cm als bak vol is (bijv. 2.5 cm)
- `Zout afstand LEEG (cm)`: afstand in cm als bak leeg is (bijv. 34 cm)

Meet-interval instelbaar (5–300 seconden). Knop "Meet nu" voor directe meting.

**Aanpassen**
- Kalibreer `depth_full_cm` en `depth_empty_cm` op jouw bak via de HA-interface
- VL53L1X `Short` mode werkt tot ~130 cm; voor grotere bakken overschakelen naar `Medium` of `Long`
