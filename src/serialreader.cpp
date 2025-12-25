#include "serialreader.h"
#include "utilfuncts.h"
#include "wifi_MGMNT.h"
#include "deauth/deauth.h"

bool serialStringComplete = false;
String serialInputString = "";
bool BLESpamActive = false;

Preferences preferences;

void waitForChar(char waitChar, unsigned long timeoutMs);

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
      } else if (inChar == '\b'){
        serialInputString.remove(serialInputString.length() - 2);
      }
      Serial.print(inChar);
    }
  }
}

void serialHandleCommand(String inputCMD){
  if(inputCMD != "\n"){
    inputCMD.replace("\n", "");
    inputCMD.replace("\r", "");
    if (inputCMD == "reboot" || inputCMD == "restart") {
      Serial.println("Rebooting...\n\n");
      ESP.restart();
    } else if (inputCMD == "forcehotspot"){
      FORCE_USE_HOTSPOT = !FORCE_USE_HOTSPOT;
      Serial.print("Toggling forceHotspot... value now ");
      Serial.println(FORCE_USE_HOTSPOT ? "true" : "false");

      preferences.begin("settings", false);
      preferences.putBool("forceHotspot", FORCE_USE_HOTSPOT);
      preferences.end();
    } else if(inputCMD == "OVERWRITEALLEEPROM"){
      OVERWRITEALLEEPROM();
    } else if (inputCMD.startsWith("wifi")){
      if(inputCMD == "wifi scan"){
          Serial.println("Scanning for WiFi networks...");
          int numNetworks = WiFi.scanNetworks(false, true);
          if(numNetworks == 0){
              Serial.println("No networks found.");
          } else {
              Serial.print("Found "); Serial.print(numNetworks); Serial.println(" networks:");
              for(int i = 0; i < numNetworks; i++){
                if(WiFi.SSID(i)){
                  Serial.print(i); Serial.print(": "); Serial.print(WiFi.SSID(i)); Serial.print(" ("); Serial.print(WiFi.BSSIDstr(i)); Serial.println(")");
                } else {
                  Serial.print(i); Serial.print(": "); Serial.print(WiFi.BSSIDstr(i));
                }
              }
          }
      } else {
          Serial.println("Unrecognized wifi command");
      }
  } else if (inputCMD.startsWith("deauth")){
      int num;
      if(inputCMD == "deauth"){
          Serial.println("Use 'deauth [BSSID]' to deauth a device.");
          return;
      } else if (inputCMD.indexOf("-n") > 0){
          inputCMD.replace("-n", "");
          inputCMD.trim();
          num = inputCMD.toInt();
      }
      inputCMD.replace("deauth", "");
      inputCMD.trim();
      String bssid = WiFi.BSSIDstr(inputCMD.toInt());
      uint16_t reason = 1;
      int networkNum = inputCMD.toInt();
      Serial.print("Deauthing device with BSSID: "); 
      Serial.println(bssid);
      start_deauth(networkNum, 0, reason);
      waitForChar('c', 300000);
      stop_deauth();
    } else if (inputCMD == "help") {
      Serial.println("Valid commands:");
      Serial.println("blespam   reboot   forcehotsot   wifi   deauth   help");
    } else {
      Serial.println("Unknown command: " + inputCMD);
      Serial.println("Type 'help' for a list of valid commands");
    }
  }
  Serial.print("> ");
}