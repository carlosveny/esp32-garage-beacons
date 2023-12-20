#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "Printer.h"

// CONFIGURATION VARIABLES
const int NUMBER_OF_SCANS = 3;     // Number of retries if device is not found
const int SCAN_TIME = 5;           // In seconds
const int SLEEP_TIME = 10 * 1000;  // In milliseconds
// Crear variable que sea cuanto tiempo continuo haciendo escaneos
String ADDRESSES[] = { "E8:C0:F9:F9:26:ED" };
String KNX_ADDRESSES[] = { "P:R:1" };

// GLOBAL VARIABLES
const int TOTAL_ELEMENTS = sizeof(ADDRESSES) / sizeof(ADDRESSES[0]);
BLEScan* pBLEScan;
boolean* foundAddresses;
Printer Printer;

class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    String macAddress = advertisedDevice.getAddress().toString().c_str();
    macAddress.toUpperCase();
    for (int i = 0; i <= TOTAL_ELEMENTS; i++) {
      if (macAddress == ADDRESSES[i]) {
        Serial.print(Printer.sprintf("Device name: %s", advertisedDevice.haveName() ? advertisedDevice.getName().c_str() : "null"));
        Serial.println(Printer.sprintf(" | MAC Address: %s", advertisedDevice.getAddress().toString().c_str()));
        if (!foundAddresses[i]) {
          foundAddresses[i] = true;
          Serial.println(Printer.sprintf("[KNX] Sent 1 to %s", KNX_ADDRESSES[i]));
          // Send here the signal (1) to KNX
        }
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n------------------------------------------------");
  Serial.println("[INFO] esp32-garage-beacons started");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);  // Active scan uses more power, but get results faster
  pBLEScan->setInterval(2000);
  pBLEScan->setWindow(1999);  // Less or equal setInterval value
}

void loop() {
  if (areVariablesValid()) {
    resetFoundAddresses();
    for (int i = 0; i < NUMBER_OF_SCANS; i++) {
      scan(i);
      if (allAddressesFound()) {
        break;
      }
    }
    sendZerosToKnx();
    Serial.println(Printer.sprintf("[INFO] Sleeping for %i milliseconds", SLEEP_TIME));
    delay(SLEEP_TIME);
  } else {
    Serial.println("[ERROR] Variables 'ADDRESSES' and 'KNX_ADDRESSES' should have the same length");
    delay(60000);
  }
}

// Function that checks if variables are valid
boolean areVariablesValid() {
  return TOTAL_ELEMENTS == (sizeof(KNX_ADDRESSES) / sizeof(KNX_ADDRESSES[0]));
}

// Function that set all positions of foundAddresses to false
void resetFoundAddresses() {
  foundAddresses = new boolean[TOTAL_ELEMENTS];
  for (int i = 0; i < TOTAL_ELEMENTS; i++) {
    foundAddresses[i] = false;
  }
}

// Function that performs a BLE Scan
void scan(int idx) {
  Serial.println(Printer.sprintf("\n[INFO] Start scan %i", idx + 1));
  BLEScanResults foundDevices = pBLEScan->start(SCAN_TIME, false);
  Serial.println(Printer.sprintf("[INFO] Scan %i done! Devices found: %i", idx + 1, foundDevices.getCount()));
  pBLEScan->clearResults();
}

// Function to check if all adresses have been found
boolean allAddressesFound() {
  boolean foundAllAddresses = true;
  for (int j = 0; j < TOTAL_ELEMENTS; j++) {
    if (!foundAddresses[j]) {
      foundAllAddresses = false;
      break;
    }
  }
  return foundAllAddresses;
}

// Function that sends zeros to the unfound devices
void sendZerosToKnx() {
  Serial.println("\n[INFO] Scans done, sending zeros to KNX");
  for (int i = 0; i < TOTAL_ELEMENTS; i++) {
    if (!foundAddresses[i]) {
      Serial.println(Printer.sprintf("[KNX] Sent 0 to %s", KNX_ADDRESSES[i]));
      // Send here the signal (0) to KNX
    }
  }
}
