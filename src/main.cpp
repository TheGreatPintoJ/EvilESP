#include <Arduino.h>
#include <TelnetStream.h>

#include "wifi_MGMNT.h"
#include "serialreader.h"
#include "espui_functs.h"
#include "evilblefuncts.h"
#include "telnetreader.h"

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
  TelnetStream.begin();
  EvilJuiceSetup();
  setUpESPUI();

  Serial.print("> ");
}

void loop() {
  serialCommandRead();
  telnetCommandRead();
  if(BLESpamActive) EvilJuiceLoop();
}