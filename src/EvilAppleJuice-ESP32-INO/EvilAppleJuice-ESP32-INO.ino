// This example takes heavy inpsiration from the ESP32 example by ronaldstoner
// Based on the previous work of chipik / _hexway / ECTO-1A & SAY-10
// See the README for more info
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_arduino_version.h>

#include "devices.h"
#include "evilblefuncts.h"

// Bluetooth maximum transmit power
#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C2) || defined(CONFIG_IDF_TARGET_ESP32S3)
#define MAX_TX_POWER ESP_PWR_LVL_P21  // ESP32C3 ESP32C2 ESP32S3
#elif defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32C6)
#define MAX_TX_POWER ESP_PWR_LVL_P20  // ESP32H2 ESP32C6
#else
#define MAX_TX_POWER ESP_PWR_LVL_P9   // Default
#endif

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32");

  EvilJuiceSetup();
}

void loop() {
  EvilJuiceLoop();
}
