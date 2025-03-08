#ifndef SERIALREADER_H
#define SERIALREADER_H

#include <Arduino.h>

extern String serialInputString;
extern bool BLESpamActive;
extern bool serialStringComplete;

void serialCommandRead();
void serialHandleCommand(String inputCMD);

#endif