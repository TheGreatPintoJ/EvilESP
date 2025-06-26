#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <regex>
#include <GamepadDevice.h>
#include <BleCompositeHID.h>

#include "bleutils.h"

BLEScan* pBLEScan;
BLEAdvertising* pAdvertising;
std::vector<BLEClient*> connectedClients;
static BleCompositeHID* compositeHID = nullptr;
static GamepadDevice* gamepad = nullptr;
static GamepadConfiguration bleGamepadConfig;
static bool compositeHIDActive = false;
static bool bleInited = false;

namespace CustomBLEUtils {
    // Scan for BLE devices
    std::vector<BLEAdvertisedDevice> scanForDevices(int scanTime = 5) {
        Serial.println("[BLE] Scanning for devices...");
        std::vector<BLEAdvertisedDevice> foundDevices;

        pBLEScan = BLEDevice::getScan();
        pBLEScan->setActiveScan(true);
        BLEScanResults scanResults = pBLEScan->start(scanTime, false);

        for (int i = 0; i < scanResults.getCount(); ++i) {
            BLEAdvertisedDevice device = scanResults.getDevice(i);
            Serial.printf("[BLE] Found Device: %s | RSSI: %d\n", device.toString().c_str(), device.getRSSI());
            foundDevices.push_back(device);
        }

        pBLEScan->clearResults();
        return foundDevices;
    }

    std::vector<BLEAddress> getAddresses(std::vector<BLEAdvertisedDevice> devices){
        std::vector<BLEAddress> addresses;
        for (auto& device : devices) {
            addresses.push_back(device.getAddress());
        }
        return addresses;
    }

    bool isValidMacAddress(const std::string& mac) {
        // Regular expression for a valid MAC address (e.g., "00:1A:2B:3C:4D:5E")
        const std::regex macRegex("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$");
        return std::regex_match(mac, macRegex);
    }

    // Pair with a BLE device (Note: ESP32 doesn't support BLE pairing in Arduino easily)
    bool pairWithDevice(std::string macAddress) {
        Serial.printf("[BLE] Attempting to pair with device: %s\n", macAddress.c_str());
        BLEAddress address(macAddress);
        BLEClient* pClient = BLEDevice::createClient();
        if (pClient->connect(address)) {
          Serial.println("[BLE] Connected. Pairing on ESP32 requires bonding to be handled in firmware or callbacks.");
          if (pClient->isConnected()) {
            Serial.println("[BLE] Device is connected. You may need to enable bonding in your app/device.");
            connectedClients.push_back(pClient);
            return true;
          }
        }
        Serial.println("[BLE] Pairing failed.");
        return false;
    }

    bool connectToDevice(std::string macAddress) {
        Serial.printf("[BLE] Connecting to device: %s\n", macAddress.c_str());
        BLEAddress address(macAddress);
        BLEClient* pClient = BLEDevice::createClient();
        if (pClient->connect(address)) {
          Serial.println("[BLE] Connected successfully.");
          connectedClients.push_back(pClient);
          return true;
        } else {
          Serial.println("[BLE] Connection failed.");
          return false;
        }
    }
    void disconnectDevice(std::string macAddress) {
        Serial.printf("[BLE] Disconnecting device: %s\n", macAddress.c_str());
        BLEAddress address(macAddress);
        BLEClient* pClient = BLEDevice::createClient();
        pClient->disconnect();
    }

    // Start advertising BLE
    void startAdvertising(String name, int type) {
        if (!bleInited) {
            BLEDevice::init(name.c_str());
            bleInited = true;
        }
        // Only end if active
        if (compositeHIDActive && compositeHID) {
            compositeHID->end();
            compositeHIDActive = false;
        }
        switch(type) {
            case 0: // Keyboard
                Serial.println("[BLE] Advertising as BLE Keyboard (placeholder)");
                // To switch to keyboard/mouse, recommend ESP.restart() for stability
                break;
            case 1: // Mouse
                Serial.println("[BLE] Advertising as BLE Mouse (placeholder)");
                // To switch to keyboard/mouse, recommend ESP.restart() for stability
                break;
            case 2: // Keyboard & Mouse
                Serial.println("[BLE] Advertising as BLE Keyboard & Mouse (placeholder)");
                // To switch to keyboard/mouse, recommend ESP.restart() for stability
                break;
            case 3: // Gamepad
                Serial.println("[BLE] Advertising as BLE Gamepad (BleCompositeHID)");
                if (!compositeHID) {
                    Serial.println("[DEBUG] Creating new BleCompositeHID");
                    compositeHID = new BleCompositeHID();
                }
                if (!gamepad) {
                    Serial.println("[DEBUG] Creating new GamepadDevice");
                    bleGamepadConfig.setAutoReport(false);
                    bleGamepadConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD);
                    bleGamepadConfig.setButtonCount(16);
                    bleGamepadConfig.setHatSwitchCount(1);
                    bleGamepadConfig.setAxesMin(0x0000);
                    bleGamepadConfig.setAxesMax(0x7FFF);
                    gamepad = new GamepadDevice(bleGamepadConfig);
                    compositeHID->addDevice(gamepad);
                }
                if (!compositeHIDActive) {
                    Serial.println("[DEBUG] Beginning gamepad...");
                    compositeHID->begin();
                    compositeHIDActive = true;
                }
                Serial.println("Done");
                break;
            default:
                Serial.println("[BLE] Unknown advertising type.");
                break;
        }
        Serial.println("[DEBUG] Exiting startAdvertising");
        delay(5000);
    }

    // Stop advertising BLE
    void stopAdvertising() {
        if (compositeHIDActive && compositeHID) {
            compositeHID->end();
            compositeHIDActive = false;
        }
        // Do not delete compositeHID/gamepad, keep them for stability
        // Loop through all types and stop advertising for each
        for (int type = 0; type <= 3; ++type) {
            switch(type) {
                case 0: // Keyboard
                    // TODO: Stop BLE Keyboard advertising with BleCompositeHID
                    Serial.println("[BLE] Stopping BLE Keyboard advertising (placeholder)");
                    break;
                case 1: // Mouse
                    // TODO: Stop BLE Mouse advertising with BleCompositeHID
                    Serial.println("[BLE] Stopping BLE Mouse advertising (placeholder)");
                    break;
                case 2: // Keyboard & Mouse
                    // TODO: Stop BLE Keyboard & Mouse advertising with BleCompositeHID
                    Serial.println("[BLE] Stopping BLE Keyboard & Mouse advertising (placeholder)");
                    break;
                case 3: // Gamepad
                    Serial.println("[BLE] Stopping BLE Gamepad advertising (BleCompositeHID)");
                    // Already handled above
                    break;
                default:
                    break;
            }
        }
        if (pAdvertising) {
            pAdvertising->stop();
            Serial.println("[BLE] Advertising stopped.");
        }
    }

    // Return a list of connected devices
    std::vector<std::string> getConnectedDevices() {
        std::vector<std::string> deviceList;
        for (auto* client : connectedClients) {
            if (client->isConnected()) {
                std::string addr = client->getPeerAddress().toString();
                Serial.printf("[BLE] Connected device: %s\n", addr.c_str());
                deviceList.push_back(addr);
            }
        }
        if(connectedClients.size() > 0){
            return deviceList;
        } else {
            return deviceList;
        }
    }
  
    // Return a list of paired devices (Note: no persistent bonding info API available)
    std::vector<std::string> getPairedDevices() {
        // Same as connected devices for now
        Serial.println("[BLE] Returning paired devices (same as connected for now).");
        return getConnectedDevices();
    }
}