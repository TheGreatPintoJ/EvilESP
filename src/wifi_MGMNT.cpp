#include "wifi_MGMNT.h"
#include "utilfuncts.h"
#include "espui_functs.h"

#define HOSTNAME "ESP32S3"

bool interfaceAP;

const int CONNECT_TIMEOUT = 10000;  // 10 seconds timeout
bool FORCE_USE_HOTSPOT = false;
const String HOTSPOT_SSID = "ESP32 Hotspot";
const String HOTSPOT_PASS = "evilesp32";

String foundNetworks[20];

void startAP(const String& ap_ssid, const String& ap_pass) {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid.c_str(), ap_pass.c_str());
    Serial.println("Starting AP...");
    
    // Wait for AP to initialize
    while (WiFi.softAPgetStationNum() == 0 && WiFi.softAPIP() == IPAddress(0, 0, 0, 0)) {
        delay(500);
        Serial.print(",");
    }
    Serial.println("\nAP Started");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.println("SSID: " + ap_ssid + ", Pass: " + ap_pass);
}

void connectWiFi() {
    String ssid = "", pass = "";

    //FORCE_USE_HOTSPOT = EEPROM.readBool(100);

    readStringFromEEPROM(ssid, 0, 32);
    readStringFromEEPROM(pass, 33, 97);

    //pass = EEPROM.readString(33);

    //pass.reserve(64);
    //for(int j = 0; j < EEPROM.length(); j++) {char c = EEPROM.read(j); if(j < 34 || j > 97) c = ' '; pass += c; delay(50);}
    //pass.replace(" ", "");
    Serial.println("Read string from EEPROM (pass): " + pass);

    if(!FORCE_USE_HOTSPOT){
        Serial.println("Attempting to connect to WiFi...");
        WiFi.begin(ssid.c_str(), pass.c_str());
        unsigned long startAttemptTime = millis();

        // Try connecting to stored WiFi credentials
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < CONNECT_TIMEOUT) {
            delay(500);
            Serial.print(".");
        }
    
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nWiFi Connected");
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
            interfaceAP = false;
        } else {
            Serial.println("\nWiFi Connection Failed, starting AP...");
            startAP(HOTSPOT_SSID, HOTSPOT_PASS);  // Fallback to AP mode
            interfaceAP = true;
        }
    } else {
        Serial.println("\nStarting AP: forcing hotspot...");
        startAP(HOTSPOT_SSID, HOTSPOT_PASS);
        interfaceAP = true;
    }
}
