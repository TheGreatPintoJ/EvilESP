#include "utilfuncts.h"

Preferences preferences;

void readStringFromEEPROM(String& buf, int baseaddress, int size) {
	Preferences preferences;
	preferences.begin("storage", true);

	buf = preferences.getString("data", "");
	preferences.end();

	Serial.println("Read string from Preferences: " + buf);
}

void OVERWRITEALLEEPROM(){
	Preferences preferences;
	preferences.begin("storage", false);
	preferences.clear();
	preferences.end();

	Serial.println("Cleared all data in Preferences.");
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