#ifndef BLEUTILS_H
#define BLEUTILS_H

#include <Arduino.h>
#include <BLEDevice.h>

namespace CustomBLEUtils{
    std::vector<BLEAdvertisedDevice> scanForDevices(int scanTime);
    std::vector<BLEAddress> getAddresses(std::vector<BLEAdvertisedDevice> devices);
    bool isValidMacAddress(const std::string& mac);
    bool pairWithDevice(std::string macAddress);
    bool connectToDevice(std::string macAddress);
    void startAdvertising();
    void stopAdvertising();
    std::vector<std::string> getConnectedDevices();
    std::vector<std::string> getPairedDevices();
}

#endif