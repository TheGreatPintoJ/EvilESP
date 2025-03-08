#ifndef UTILFUNCTS_H
#define UTILFUNCTS_H

#include <Arduino.h>
#include <EEPROM.h>

void readStringFromEEPROM(String& buf, int baseaddress, int size);


#endif