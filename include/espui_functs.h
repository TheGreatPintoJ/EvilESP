#ifndef ESPUI_FUNCTS_H
#define ESPUI_FUNCTS_H

#include <Arduino.h>
#include <Preferences.h>
#include <ESPUI.h>

#include <WiFi.h>
#include <ESPmDNS.h>

// Type-based callbacks
void updateCallback(Control *sender, int type);
void getTimeCallback(Control *sender, int type);
void textCallback(Control *sender, int type);
void generalCallback(Control *sender, int type);
void extendedCallback(Control* sender, int type, void* param);

// BLE-based callbacks
void updateBLESpamStatus(bool active);
void blespamSwitchCallback(Control *sender, int type);
void scanBLECallback(Control *sender, int type);
void connectBLECallback(Control *sender, int type);
void keyPressCallback(Control *sender, int type);
void advertCallback(Control *sender, int type);

// WiFi-based callbacks
void setForceHotspot(bool value);
void setForceHotspotCallback(Control *sender, int type);
void scanWiFi(Control *sender, int type);
void enterWifiDetailsCallback(Control *sender, int type);
void startDeauthCallback(Control *sender, int type);
void stopDeauthCallback(Control *sender, int type);

// Miscellaneous
void setUpESPUI();
void randomString(char *buf, int len);

#endif