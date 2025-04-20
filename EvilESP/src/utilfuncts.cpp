#include "utilfuncts.h"

void readStringFromEEPROM(String& buf, int baseaddress, int size) {
	EEPROM.begin(size);
	buf = "";
	//buf.reserve(size);
	for (int i = baseaddress; i < baseaddress+size; i++) {
		char c = EEPROM.read(i);
		if(!c) break;
		buf += c;
	}
	EEPROM.end();
	delay(500);
	Serial.println("Read string from EEPROM: " + buf);
}

void OVERWRITEALLEEPROM(){
	EEPROM.begin(4096);
	for(int i = 0; i < EEPROM.length(); i++) EEPROM.write(i, 0xFF);
	delay(1000);
	EEPROM.end();
}

void waitForChar(char waitChar, unsigned long timeoutMs) {
	unsigned long startTime = millis();
	while (millis() - startTime < timeoutMs) {
		if (Serial.available()) {
			char inChar = Serial.read();
			if (inChar == waitChar) return;
		}
	}
	Serial.println("Timeout waiting for character.");
  }