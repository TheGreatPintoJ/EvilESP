#ifndef WIFI_MGMNT_H
#define WIFI_MGMNT_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <EEPROM.h>

extern char HOSTNAME;
extern bool FORCE_USE_HOTSPOT;
extern String hotspot_ssid, hotspot_pass;

void connectWifi();
void startAP(String ssid, String pass);

#endif