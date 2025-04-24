#include <TelnetStream.h>

#include "utilfuncts.h"
#include "telnetreader.h"
#include "serialreader.h"
#include "wifi_MGMNT.h"
#include "bleutils.h"
#include "deauth/deauth.h"

Preferences preferences;

bool stringComplete = false;
String telnetString = "";

std::vector<BLEAdvertisedDevice> bleList;

void telnetCommandRead(){
    if(stringComplete){
        telnetHandleCommand(telnetString);
        telnetString = "";
        stringComplete = false;
        TelnetStream.print("> ");
    } else {
        if(TelnetStream.available()){
            char input = TelnetStream.read();
            if(input == '\n' || input == '\r'){
                stringComplete = true;
            } else if(input == '\b'){
                telnetString.remove(telnetString.length() - 1);
            } else if(' ' <= input && input <= '~'){
                // Only add printable characters to the string
                telnetString += input;
            }
        }
    }
}

void telnetHandleCommand(String inputCMD){
    TelnetStream.println();
    char buffer[inputCMD.length() + 1];
    inputCMD.toCharArray(buffer, 50, 0);
    inputCMD.trim();
    inputCMD.replace("�", "");
    inputCMD.toLowerCase();
    if(inputCMD == ""){
    } else if(inputCMD == "restart" || inputCMD == "reboot"){
        TelnetStream.println("Rebooting...");
        ESP.restart();
    } else if (inputCMD.startsWith("blespam")){
        TelnetStream.print("Status: ");
        TelnetStream.println(BLESpamActive ? "true":"false");
        if(inputCMD == "blespam") TelnetStream.println("Type 'blespam help' for help on this command");
        if(inputCMD == "blespam start"){
            TelnetStream.println("Starting blespam...");
            BLESpamActive = true;
        } else if(inputCMD == "blespam stop"){
            TelnetStream.println("Stopping blespam...");
            BLESpamActive = false;
        } else if(inputCMD == "blespam help") TelnetStream.println("BLE Spam help\nUsage: blespam [ start | stop ]");
    } else if (inputCMD.startsWith("ble")){
        if(inputCMD.startsWith("ble scan")){
            inputCMD.replace("ble scan", "");
            inputCMD.replace(" ", "");
            inputCMD.replace("-", "");
            TelnetStream.println("Scanning for BLE devices...");
            bleList = CustomBLEUtils::scanForDevices(10);
            bool onlyNamed = true;

            if(inputCMD.indexOf("a") > 0) onlyNamed = false;

            if(inputCMD == ""){
                for(int i = 0; i < bleList.size(); i++){
                    if(!onlyNamed || bleList[i].haveName()){
                        TelnetStream.print("Found device: ");
                        TelnetStream.print(bleList[i].getName().c_str());
                        TelnetStream.print(" with mac: ");
                        TelnetStream.print(bleList[i].getAddress().toString().c_str());
                        TelnetStream.println();
                    }
                }
            }
        } else if (inputCMD.startsWith("ble connect")){
            inputCMD.replace("ble connect", "");
            inputCMD.replace(" ", "");
            if (inputCMD == ""){
                TelnetStream.println("Use 'ble connect [MAC]' to connnect to a device.");
            } else {
                if(CustomBLEUtils::connectToDevice(inputCMD.c_str())){
                    TelnetStream.println("Successfully connected to device with mac address of " + inputCMD);
                } else TelnetStream.println("Failed to connect to device with mac address of " + inputCMD);
            }
        } else if (inputCMD.startsWith("ble pair")){
            inputCMD.replace("ble pair", "");
            inputCMD.replace(" ", "");
            if (inputCMD == ""){
                TelnetStream.println("Use 'ble pair [MAC]' to pair with a device.");
            } else {
                if(CustomBLEUtils::pairWithDevice(inputCMD.c_str())){
                    TelnetStream.println("Successfully paired to device with mac address of " + inputCMD);
                } else TelnetStream.println("Failed to connect to decice with mac address of " + inputCMD);
            }
        } else if (inputCMD.startsWith("ble list")){
            auto connectedDevices = CustomBLEUtils::getConnectedDevices();
            if (inputCMD == "ble list"){
                if(connectedDevices.size() > 0){
                    TelnetStream.println("Connected to the following devices: ");
                    for(int i = 0; i <= connectedDevices.size(); i++){
                        TelnetStream.print("Name: "); TelnetStream.print(connectedDevices[i].c_str());
                        TelnetStream.println();
                    }
                } else TelnetStream.println("No connected devices.");
            } else TelnetStream.println("Unrecognized ble command");
        } else TelnetStream.println("Unrecognized ble command");
    } else if (inputCMD == "forcehotspot"){
        FORCE_USE_HOTSPOT = !FORCE_USE_HOTSPOT;
        TelnetStream.print("Toggling forceHotspot... value now ");
        TelnetStream.println(FORCE_USE_HOTSPOT ? "true" : "false");

        preferences.begin("settings", false);
        preferences.putBool("forceHotspot", FORCE_USE_HOTSPOT);
        preferences.end();
    } else if (inputCMD.startsWith("wifi")){
        if(inputCMD == "wifi scan"){
            TelnetStream.println("Scanning for WiFi networks...");
            int numNetworks = WiFi.scanNetworks(false, true);
            if(numNetworks == 0){
                TelnetStream.println("No networks found.");
            } else {
                TelnetStream.print("Found "); TelnetStream.print(numNetworks); TelnetStream.println(" networks:");
                for(int i = 0; i < numNetworks; i++){
                    TelnetStream.print(i); TelnetStream.print(": "); TelnetStream.print(WiFi.SSID(i)); TelnetStream.print(" ("); TelnetStream.print(WiFi.BSSIDstr(i)); TelnetStream.println(")");
                }
            }
        } else {
            TelnetStream.println("Unrecognized wifi command");
        }
    } else if (inputCMD.startsWith("deauth")){
        int num;
        if(inputCMD == "deauth"){
            TelnetStream.println("Use 'deauth [BSSID]' to deauth a device.");
            return;
        } else if (inputCMD.indexOf("-n") > 0){
            inputCMD.replace("-n", "");
            inputCMD.trim();
            num = inputCMD.toInt();
        }
        inputCMD.replace("deauth", "");
        inputCMD.trim();
        String bssid = WiFi.BSSIDstr(inputCMD.toInt());
        TelnetStream.print("Deauthing device with BSSID: "); 
        TelnetStream.println(bssid);
        start_deauth(0, 0x0004);
        waitForChar('c', 300000);
        stop_deauth();
    } else if (inputCMD.startsWith("sleep")){
        if(inputCMD == "sleep"){
            TelnetStream.println("Use 'sleep [seconds]' to put the device to sleep.");
            return;
        }
        inputCMD.replace("sleep", "");
        inputCMD.replace(" ", "");
        long duration = inputCMD.toInt();
        TelnetStream.print("Sleeping for "); 
        TelnetStream.print(duration); 
        TelnetStream.println(" seconds...");
        delay(1000);
        esp_sleep_enable_wifi_wakeup();
        esp_deep_sleep(duration * 1000000L); // Convert seconds to microseconds
    } else if (inputCMD == "help") {
        TelnetStream.println("Valid commands:");
        TelnetStream.println("blespam   reboot   forcehotsot   help");
    } else {
        TelnetStream.println("Unknown command: " + inputCMD);
        TelnetStream.println("Type 'help' for a list of valid commands");
    }
}