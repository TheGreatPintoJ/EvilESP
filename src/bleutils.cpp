#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <regex>

#include "bleutils.h"

BLEScan* pBLEScan;
BLEAdvertising* pAdvertising;
std::vector<BLEClient*> connectedClients;


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

    // Start advertising BLE
    void startAdvertising() {
        BLEDevice::init("ESP32_BLE_Device");
        pAdvertising = BLEDevice::getAdvertising();
        BLEServer* pServer = BLEDevice::createServer();
        BLEService* pService = pServer->createService("180A");
        BLECharacteristic* pCharacteristic = pService->createCharacteristic("2A29", BLECharacteristic::PROPERTY_READ);
        pCharacteristic->setValue("ESP32");
        pService->start();
        pAdvertising->start();
        Serial.println("[BLE] Advertising started.");
    }

    // Stop advertising BLE
    void stopAdvertising() {
        if (pAdvertising) {
            pAdvertising->stop();
            Serial.println("[BLE] Advertising stopped.");
        }
    }

    // Return a list of connected devices
    std::vector<std::string> getConnectedDevices() {
        std::vector<std::string> deviceList;
        for (BLEClient* client : connectedClients) {
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