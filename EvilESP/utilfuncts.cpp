#include "utilfuncts.h"

void readStringFromEEPROM(String& buf, int baseaddress, int size) {
	buf.reserve(size);
	for (int i = baseaddress; i < baseaddress+size; i++) {
		char c = EEPROM.read(i);
		buf += c;
		if(!c) break;
	}
  //Serial.println("Read string \"" + buf + "\" from EEPROM");
}