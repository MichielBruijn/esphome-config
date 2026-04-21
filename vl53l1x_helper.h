#pragma once
#include "esphome.h"
#include <Wire.h>
#include <VL53L1X.h>

static VL53L1X tof;
static bool bus_ok = false;
static bool tof_ok = false;

static void vl53_setup() {
  if (!bus_ok) {
    Wire.begin(4, 3);
    Wire.setClock(100000);
    bus_ok = true;
  }
  if (!tof_ok) {
    tof.setTimeout(50);
    if (!tof.init()) {
      ESP_LOGW("vl53", "tof.init() failed (addr 0x29?)");
      tof_ok = false;
      return;
    }
    tof.setDistanceMode(VL53L1X::Short);
    tof.setMeasurementTimingBudget(50000);
    tof.startContinuous(60);
    tof_ok = true;
    ESP_LOGI("vl53", "VL53L1X init ok");
  }
}

static void publish_zoutafstand() {
  vl53_setup();
  if (!tof_ok) { id(zout_afstand_cm).publish_state(NAN); return; }

  uint16_t mm = tof.read();
  if (tof.timeoutOccurred() || mm == 0 || mm > 4000) {
    id(zout_afstand_cm).publish_state(NAN);
    if (tof.timeoutOccurred()) tof_ok = false;
    return;
  }

  const float cm = mm / 10.0f;
  id(zout_afstand_cm).publish_state(cm);
}
