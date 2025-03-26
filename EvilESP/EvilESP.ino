#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_arduino_version.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <NetworkUdp.h>
#include <ArduinoOTA.h>
#include <ESPUI.h>
#include <AsyncTCP.h>

#include "devices.h"
#include "evilblefuncts.h"
#include "serialreader.h"
#include "arduinoOTA_MGMNT.h"
#include "wifi_MGMNT.h"
#include "utilfuncts.h"
#include "espui_functs.h"

void setup() {
  Serial.begin(9600);
  Serial.println("Starting ESP32");

  connectWifi();
  setupOTAUpdate();

  while (WiFi.status() != WL_CONNECTED) {
        delay(100);
    }
  Serial.println("WiFi Connected");
  delay(2000);
  setUpESPUI();
  //ESPUI.begin("EvilESP");

  EvilJuiceSetup();
}

void loop() {
  ArduinoOTA.handle();
  serialCommandRead();

  if (BLESpamActive) {
    EvilJuiceLoop();
  }
}
