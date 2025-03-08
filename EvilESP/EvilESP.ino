#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_arduino_version.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <NetworkUdp.h>
#include <ArduinoOTA.h>

#include "devices.h"
#include "evilblefuncts.h"
#include "serialreader.h"
#include "arduinoOTA.h"
#include "wifi_MGMNT.h"

void setup() {
  Serial.begin(9600);
  Serial.println("Starting ESP32");

  setupOTA();
  connectWifi();

  EvilJuiceSetup();
}

void loop() {
  ArduinoOTA.handle();
  serialCommandRead();

  if (BLESpamActive) {
    EvilJuiceLoop();
  }
}
