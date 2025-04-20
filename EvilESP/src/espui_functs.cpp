#include "espui_functs.h"
#include "serialreader.h"
#include "wifi_MGMNT.h"
#include "utilfuncts.h"
#include "bleutils.h"
#include "deauth/deauth.h"

uint16_t wifi_ssid_text, wifi_pass_text;
uint16_t mainLabel, mainSwitcher, mainSlider, mainText, mainNumber, mainScrambleButton, mainTime;
uint16_t styleButton, styleLabel, styleSwitcher, styleSlider, styleButton2, styleLabel2, styleSlider2;
uint16_t graph;
uint16_t blespamStatusDash, blespamStatus, blespamSwitch, bleScanSelector, bleScanButton, bleConnectButton, bleShowMacSwitch;
uint16_t deauthPanel;
volatile bool updates = false;

std::vector<BLEAdvertisedDevice> scannedBLEDevices;

void setUpESPUI() {
	//Turn off verbose debugging
	ESPUI.setVerbosity(Verbosity::Quiet);

	//Make sliders continually report their position as they are being dragged.
	ESPUI.sliderContinuous = true;

	/*
	 * Tab: Dashboard
	 *-----------------------------------------------------------------------------------------------------------*/
	auto dashboardTab = ESPUI.addControl(Tab, "", "Dashboard");
	blespamStatusDash = ESPUI.addControl(Label, "BLESpam status", "Off", Peterriver, dashboardTab, generalCallback);

	/*
	 * Tab: Bluetooth
	 *-----------------------------------------------------------------------------------------------------------*/
	auto bluetoothTab = ESPUI.addControl(Tab, "", "Bluetooth");
	blespamStatus = ESPUI.addControl(Label, "BLESpam status", "Off", Peterriver, bluetoothTab, generalCallback);
	blespamSwitch = ESPUI.addControl(Switcher, "", (String)BLESpamActive, ControlColor::Peterriver, blespamStatus, blespamSwitchCallback);
	ESPUI.addControl(Separator, "General BLE controls", "", None, bluetoothTab);
	bleScanSelector = ESPUI.addControl(Select, "BLE Devices", "Scan below", Peterriver, bluetoothTab);
	bleScanButton = ESPUI.addControl(Button, "", "Scan", Peterriver, bleScanSelector, scanBLECallback);
	bleConnectButton = ESPUI.addControl(Button, "", "Connect", Peterriver, bleScanSelector, connectBLECallback);
	bleShowMacSwitch = ESPUI.addControl(Switcher, "Show MAC", "0", Peterriver, bleScanSelector, textCallback);

	ESPUI.setElementStyle(bleShowMacSwitch, "position: absolute; margin-top:6px;"); //Move the MAC switch down a bit to make it look better
	
	/*
	 * Tab: WiFi
	 *-----------------------------------------------------------------------------------------------------------*/
	uint16_t wifiTab = ESPUI.addControl(Tab, "", "WiFi");

	//** Attacks */
	ESPUI.addControl(Separator, "Attacks", "", None, wifiTab);
	deauthPanel = ESPUI.addControl(Select, "Deauth", "Select target", Alizarin, wifiTab);
	auto startDeauthBtn = ESPUI.addControl(Button, "Deauth", "Start", Alizarin, deauthPanel, startDeauthCallback);
	auto stopDeauthBtn = ESPUI.addControl(Button, "Deauth", "Stop", Alizarin, deauthPanel, stopDeauthCallback);
	ESPUI.addControl(Button, "Deauth", "Scan", Alizarin, deauthPanel, scanWiFi);

	if(interfaceAP){
		ESPUI.setEnabled(startDeauthBtn, false);
		ESPUI.setEnabled(stopDeauthBtn, false);
	}

	//** Creds */
	ESPUI.addControl(Separator, "Change creds", "", None, wifiTab);
	wifi_ssid_text = ESPUI.addControl(Text, "SSID", "", Alizarin, wifiTab, textCallback);
	//Note that adding a "Max" control to a text control sets the max length
	ESPUI.addControl(Max, "", "32", None, wifi_ssid_text);
	wifi_pass_text = ESPUI.addControl(Text, "Password", "", Alizarin, wifiTab, textCallback);
	ESPUI.addControl(Max, "", "64", None, wifi_pass_text);
	auto wifiOptions = ESPUI.addControl(Button, "Options", "Save Creds", Peterriver, wifiTab, enterWifiDetailsCallback);
	auto forceHotspotButton = ESPUI.addControl(Switcher, "Options", FORCE_USE_HOTSPOT ? "1":"0", Peterriver, wifiOptions, &setForceHotspotCallback);

	ESPUI.setElementStyle(forceHotspotButton, "position: absolute; margin-top:6px;");

	/*
	 * Tab: Utils
	 *-----------------------------------------------------------------------------------------------------------*/
	uint16_t utilsTab = ESPUI.addControl(Tab, "", "Utils");
	ESPUI.addControl(Separator, "General", "", None, utilsTab);
	uint16_t generalPanel = ESPUI.addControl(Button, "General", "Restart", Alizarin, utilsTab, [](Control *sender, int type){if(type == B_UP) ESP.restart();});
	uint16_t sleepNum = ESPUI.addControl(Number, "SleepTimer", "1", Alizarin, generalPanel, textCallback);
	ESPUI.addControl(Button, "General", "Sleep", Alizarin, generalPanel, [sleepNum](Control *sender, int type){
		if(type == B_UP) {
			long sleepTime = ESPUI.getControl(sleepNum)->value.toInt() * 1000000;
			Serial.println("Sleeping...");
			ESP.deepSleep(sleepTime);
		}
	});

	ESPUI.addControl(Max, "", "99999999999999", None, sleepNum);

	//Finally, start up the UI. 
	//This should only be called once we are connected to WiFi.
	Serial.print("Starting ESPUI");
	ESPUI.begin("EvilESP");
	Serial.println(" finished");
	ESPUI.jsonReload();
}

void scanWiFi(Control *sender, int type){
	if(type == B_DOWN){
		Serial.println("Scanning for networks...");
		WiFi.scanNetworks(true, true);
	} else if(type == B_UP){
		int numNetworks = WiFi.scanComplete();
		if(numNetworks > 0){
			for(int i = 0; i < numNetworks; i++){
				String ssid = WiFi.SSID(i);
				String bssid = WiFi.BSSIDstr(i);
				if(!ssid.isEmpty()) ESPUI.addControl(Option, strdup(ssid.c_str()), bssid, None, deauthPanel);
			}
			Serial.println("Scan complete.");
		} else {
			Serial.println("No networks found.");
		}
	}
}

void startDeauthCallback(Control *sender, int type){
	if(type == B_UP){
		auto control = ESPUI.getControl(deauthPanel);
		if (control != nullptr) {
			int selectedIndex = control->value.toInt();
			//Serial.println("Starting deauth attack on network " + String(selectedIndex));
			if(selectedIndex >= 0 && selectedIndex < WiFi.scanComplete()){
				String ssid = WiFi.SSID(selectedIndex);
				String bssid = WiFi.BSSIDstr(selectedIndex);
				Serial.print("Attacking network: ");
				Serial.println(ssid);
				Serial.print("BSSID: ");
				Serial.println(bssid);
				start_deauth(selectedIndex, (uint16_t)0x0001); // Start deauth attack
			} else {
				Serial.println("Invalid network selected.");
			}
		} else {
			Serial.println("Error: deauthPanel control is null.");
		}
	}
}

void stopDeauthCallback(Control *sender, int type){
	if(type == B_UP){
		stop_deauth(); // Stop deauth attack
	}
	ESPUI.updateLabel(deauthPanel, "Select target");
}

void setForceHotspot(bool value){
	EEPROM.begin(100); EEPROM.writeBool(100, value); EEPROM.end();
}

void setForceHotspotCallback(Control *sender, int type){
	Serial.print("[ESPUI] Toggled hotspot value to ");
	if(type == S_ACTIVE){
		setForceHotspot(true);
		Serial.println("true");
	} else if(type == S_INACTIVE){
		setForceHotspot(false);
		Serial.println("false");
	}
}

void updateBLESpamStatus(bool active) {
	ESPUI.updateLabel(blespamStatusDash, (active ? "On" : "Off"));
	ESPUI.updateLabel(blespamStatus, (active ? "On" : "Off"));
	BLESpamActive = active;
	ESPUI.updateSwitcher(blespamSwitch, active);
}

void scanBLECallback(Control *sender, int type){
	if(type == B_UP){
		bool showMac = (ESPUI.getControl(bleShowMacSwitch)->value == "1");
		scannedBLEDevices.clear();
		scannedBLEDevices = CustomBLEUtils::scanForDevices(2);
		ESPUI.updateSelect(bleScanSelector, "");
		for (int i = 0; i < scannedBLEDevices.size(); i++) {
			BLEAdvertisedDevice device = scannedBLEDevices[i];
			std::string nameStd = device.getName();
			String name = String(nameStd.c_str());
			String addr = device.getAddress().toString().c_str();
			if (name.length() > 0) {
				ESPUI.addControl(Option, strdup(name.c_str()), addr, None, bleScanSelector);
				Serial.print("Added device to select: ");
				Serial.println(name);
			} else if (showMac){
				ESPUI.addControl(Option, strdup(addr.c_str()), addr, None, bleScanSelector);
				Serial.print("Added mac device to select: ");
				Serial.println(addr);
			}
		}
	} else if(type == B_DOWN);
}

void connectBLECallback(Control *sender, int type){
	if(type == B_UP){
		std::string selectedDevice = ESPUI.getControl(bleScanSelector)->value.c_str();
		std::vector<BLEAddress> addresses = CustomBLEUtils::getAddresses(scannedBLEDevices);
		if(!CustomBLEUtils::isValidMacAddress(selectedDevice)){
			for(int i = 0; i < scannedBLEDevices.size(); i++){
				if(scannedBLEDevices[i].getName() == selectedDevice){
					selectedDevice = scannedBLEDevices[i].getAddress().toString().c_str();
					break;
				}
			}
		}
		Serial.print("Connecting to device: ");
		Serial.println(selectedDevice.c_str());
		if(CustomBLEUtils::connectToDevice(selectedDevice)){
			Serial.println("Connected successfully.");
			ESPUI.updateLabel(bleConnectButton, "Connected");
			ESPUI.updateLabel(bleConnectButton, "Disconnect");
		} else {
			Serial.println("Failed to connect.");
			ESPUI.updateLabel(bleConnectButton, "Failed to connect");
		}
		
	} else if(type == B_DOWN);
}

void blespamSwitchCallback(Control *sender, int type){
	if(type == S_ACTIVE){
		updateBLESpamStatus(true);
	} else if(type == S_INACTIVE){
		updateBLESpamStatus(false);
	}
}

void getTimeCallback(Control *sender, int type){
	if(type == B_UP) {
		ESPUI.updateTime(mainTime);
	}
}

void generalCallback(Control *sender, int type){
	Serial.print("CB: id(");
	Serial.print(sender->id);
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);
}

void extendedCallback(Control* sender, int type, void* param){
    Serial.print("CB: id(");
    Serial.print(sender->id);
    Serial.print(") Type(");
    Serial.print(type);
    Serial.print(") '");
    Serial.print(sender->label);
    Serial.print("' = ");
    Serial.println(sender->value);
    Serial.print("param = ");
    Serial.println((long)param);
}

void enterWifiDetailsCallback(Control *sender, int type){
	if(type == B_UP) {
		Serial.println("Saving credentials to EPROM...");
		String tempssid = ESPUI.getControl(wifi_ssid_text)->value;
		String temppass = ESPUI.getControl(wifi_pass_text)->value;
		Serial.println(tempssid);
		Serial.println(temppass);
		
		unsigned int i;
		EEPROM.begin(100);
		for(i = 0; i < 96; i++) {
			EEPROM.write(i, 0x00);
		}

		if (tempssid.length() <= 32 && temppass.length() <= 64) {
			EEPROM.writeString(0, tempssid);
			EEPROM.writeString(33, temppass);
		} else {
			Serial.println("Error: SSID or password exceeds maximum length.");
		}
		
		delay(1000);
		EEPROM.end();
	}
}

void textCallback(Control *sender, int type){}

void randomString(char *buf, int len){
	for(auto i = 0; i < len-1; i++) 
		buf[i] = random(0, 26) + 'A';
	buf[len-1] = '\0';
}