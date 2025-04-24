#ifndef UTILFUNCTS_H
#define UTILFUNCTS_H

#include <Arduino.h>
#include <Preferences.h>

void readStringFromEEPROM(String& buf, int baseaddress, int size);
void waitForChar(char waitChar, unsigned long timeoutMs);

void OVERWRITEALLEEPROM();

#endif