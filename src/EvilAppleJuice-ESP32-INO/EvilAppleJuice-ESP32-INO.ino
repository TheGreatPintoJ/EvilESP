#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_arduino_version.h>

#include "devices.h"
#include "evilblefuncts.h"
#include "serialreader.h"

bool BLESpamActive = false;


void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32");

  EvilJuiceSetup();
}

void loop() {
  serialCommandRead();

  if(BLESpamActive){
    EvilJuiceLoop();
  }
}
