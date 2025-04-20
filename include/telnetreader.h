#ifndef TELNETREADER
#define TELNETREADER

#include <Arduino.h>

void telnetCommandRead();
void telnetHandleCommand(String inputCMD);

#endif