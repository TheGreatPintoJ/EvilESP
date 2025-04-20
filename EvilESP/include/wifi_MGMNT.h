#ifndef WIFI_MGMNT_H
#define WIFI_MGMNT_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <EEPROM.h>

extern bool interfaceAP;

extern char HOSTNAME;
extern bool FORCE_USE_HOTSPOT;
extern String hotspot_ssid, hotspot_pass;

extern String foundNetworks[20];

void connectWiFi();
void startAP(const String& ssid, const String& pass);
void setupWiFi(String ssid, String pass);

#endif