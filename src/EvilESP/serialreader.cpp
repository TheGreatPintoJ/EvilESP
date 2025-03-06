#include "serialreader.h"

bool serialStringComplete = false;
String serialInputString = "";
bool BLESpamActive = false;

void serialCommandRead(){
  if (serialStringComplete) {
    serialHandleCommand(serialInputString);
    serialInputString = "";
    serialStringComplete = false;
  } else {
    if(Serial.available()){
      char inChar = (char)Serial.read();

      serialInputString += inChar;
      if (inChar == '\n') {
        serialStringComplete = true;
      }
    }
  }
}

void serialHandleCommand(String inputCMD){
  inputCMD.replace("\n", "");
  inputCMD.replace("\r", "");
  if (inputCMD == "reboot" || inputCMD == "restart") {
    Serial.println("Rebooting...");
    ESP.restart();
  } else if (inputCMD == "blespam stop") {
      Serial.println("Stopping BLE Spam...");
      BLESpamActive = false;
  } else if (inputCMD == "blespam start") {
      Serial.println("Starting BLE Spam...");
      BLESpamActive = true;
  } else if (inputCMD == "blespam help") {
      Serial.println("BLESpam help");
      Serial.println("blespam [ start | stop ]");
  } else if (inputCMD == "help") {
      Serial.println("Valid commands:");
      Serial.println("blespam   reboot   help");
  } else {
      Serial.println("Unknown command: " + inputCMD);
      Serial.println("Type 'help' for a list of valid commands");
  }
}