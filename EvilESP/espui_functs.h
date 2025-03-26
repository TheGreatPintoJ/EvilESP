#ifndef ESPUI_FUNCTS_H
#define ESPUI_FUNCTS_H

#include <Arduino.h>
#include <EEPROM.h>
#include <ESPUI.h>

#include <WiFi.h>
#include <ESPmDNS.h>

void setUpESPUI();

void updateCallback(Control *sender, int type);
void getTimeCallback(Control *sender, int type);
void graphAddCallback(Control *sender, int type);
void graphClearCallback(Control *sender, int type);
void randomString(char *buf, int len);
void extendedCallback(Control* sender, int type, void* param);
void enterWifiDetailsCallback(Control *sender, int type);
void textCallback(Control *sender, int type);
void generalCallback(Control *sender, int type);
void extendedCallback(Control* sender, int type, void* param);


#endif