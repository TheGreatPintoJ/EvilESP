#include "wifi_MGMNT.h"
#include "utilfuncts.h"

#define HOSTNAME "ESP32S3"

int connect_timeout = 28;  //7 seconds

bool FORCE_USE_HOTSPOT = false;
String hotspot_ssid = "ESP32 Hotspot";
String hotspot_pass = "esp32hotspot";

void connectWifi() {
  WiFi.setHostname(HOSTNAME);

  Serial.println("Begin wifi...");

  //Load credentials from EEPROM
  if (!(FORCE_USE_HOTSPOT)) {
    yield();
    EEPROM.begin(100);
    String stored_ssid, stored_pass;
    readStringFromEEPROM(stored_ssid, 0, 32);
    readStringFromEEPROM(stored_pass, 32, 96);
    EEPROM.end();

    //Try to connect with stored credentials, fire up an access point if they don't work.
    WiFi.begin(stored_ssid.c_str(), stored_pass.c_str());
    while (WiFi.status() != WL_CONNECTED && connect_timeout > 0) {
      delay(250);
      Serial.print(".");
      connect_timeout--;
    }
  } else {
    startAP(hotspot_ssid, hotspot_pass);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" Wifi started");

    if (!MDNS.begin(HOSTNAME)) {
      Serial.println("Error setting up MDNS responder!");
    }
  } else {
    startAP(HOSTNAME, "");
  }
}

void startAP(String ssid, String pass){
  Serial.println("\nCreating access point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
    WiFi.softAP(ssid, pass);

    connect_timeout = 20;
    do {
      delay(250);
      Serial.print(",");
      connect_timeout--;
    } while (connect_timeout);
}