#include <Arduino.h>

#include "wifi_MGMNT.h"
#include "serialreader.h"
#include "evilblefuncts.h"

bool SLOWBOOT = true;
long SLOWBOOT_DELAY = 1;

bool deauthActive = false;
int curr_channel = 1;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  if(SLOWBOOT) delay(SLOWBOOT_DELAY*1000);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  Serial.println("Starting EvilESP");

  connectWiFi();
  EvilJuiceSetup();

  Serial.print("> ");
}

void loop() {
  serialCommandRead();
  if(BLESpamActive) EvilJuiceLoop();
}