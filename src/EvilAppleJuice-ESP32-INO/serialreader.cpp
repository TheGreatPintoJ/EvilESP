#include "serialreader.h"

bool serialStringComplete = false;
String serialInputString = "";

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

void serialHandleCommand(std::tring inputCMD){
  switch (inputCMD) {
    case "reboot":
      Serial.println("Rebooting...");
      ESP.restart();
      break;
    case "blespam stop":
      Serial.println("Stopping BLE Spam...");
      BLESpamActive = false;
      break;
    case "blespam start":
      Serial.println("Starting BLE Spam...");
      BLESpamActive = true;
      break;
    case "blespam help":
      Serial.println("BLESpam help");
      Serial.println("blespam [ start | stop ]");
    case "help":
      Serial.println("Valid commands:");
      Serial.println("blespam   reboot   help");
    default:
      Serial.println("Unknown command: " + inputCMD);
      Serial.println("Type 'help' for a list of valid commands");
      break;
  }
}