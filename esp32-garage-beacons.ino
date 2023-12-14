#include <Arduino.h>

#include <BLEDevice.h>
// #include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
// #include <BLEEddystoneURL.h>
// #include <BLEEddystoneTLM.h>
// #include <BLEBeacon.h>

BLEScan *pBLEScan;
const int SCAN_TIME = 6; // In seconds
String ADDRESSES[] = { "hola", "E8:C0:F9:F9:26:ED" };

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      String macAddress = advertisedDevice.getAddress().toString().c_str();
      macAddress.toUpperCase();
      Serial.println(sizeof(ADDRESSES));
      for (int i = 0; i <= sizeof(ADDRESSES); i++) {
        if (macAddress == ADDRESSES[i]) {
          Serial.println("CORRECTO");
        }
      }

      if (advertisedDevice.haveName()) {
        Serial.print("Device name: ");
        Serial.println(advertisedDevice.getName().c_str());
        Serial.print("MAC Address: ");
        Serial.println(advertisedDevice.getAddress().toString().c_str());
        Serial.println("");
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); // Create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); // Active scan uses more power, but get results faster
  pBLEScan->setInterval(2000);
  pBLEScan->setWindow(1999); // Less or equal setInterval value
}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(SCAN_TIME, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults(); // Delete results fromBLEScan buffer to release memory
  delay(2000);
}
