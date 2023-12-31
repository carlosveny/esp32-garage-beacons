#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <KnxTpUart.h>
#include "Printer/Printer.h"
#include "Config/Config.h"

const int TOTAL_ELEMENTS = sizeof(BLE_ADDRESSES) / sizeof(BLE_ADDRESSES[0]);
const int TOTAL_SCANS = TOTAL_SECONDS_SCANNING / (NUMBER_OF_SCANS * SCAN_TIME);
BLEScan* pBLEScan;
boolean* foundAddresses;
Printer Printer(60);
KnxTpUart knx(&Serial2, "15.15.232");  // Initialize the KNX TP-UART library on the Serial port

class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    String macAddress = advertisedDevice.getAddress().toString().c_str();
    macAddress.toUpperCase();
    for (int i = 0; i <= TOTAL_ELEMENTS; i++) {
      if (macAddress == BLE_ADDRESSES[i]) {
        Serial.print(Printer.sprintf("\t\t[BLE] Device name: %s", advertisedDevice.haveName() ? advertisedDevice.getName().c_str() : "null"));
        Serial.println(Printer.sprintf(" | MAC Address: %s", advertisedDevice.getAddress().toString().c_str()));
        if (!foundAddresses[i]) {
          foundAddresses[i] = true;
          Serial.println(Printer.sprintf("\t\t[KNX] Sent 1 to %s", KNX_ADDRESSES[i]));
          knx.groupWriteBool(KNX_ADDRESSES[i], true);
        }
      }
    }
  }
};

void serialEvent2() {
  KnxTpUartSerialEventType eType = knx.serialEvent();
  if (eType == TPUART_RESET_INDICATION) {
    Serial.println("\n[KNX] Waiting for KNX signal to start scanning...");
  } else if (eType == KNX_TELEGRAM) {
    KnxTelegram* telegram = knx.getReceivedTelegram();
    String target = String(0 + telegram->getTargetMainGroup()) + "/" + String(0 + telegram->getTargetMiddleGroup()) + "/" + String(0 + telegram->getTargetSubGroup());

    if (telegram->getCommand() == KNX_COMMAND_WRITE) {
      bool targetValue = telegram->getBool();
      if (targetValue) {
        Serial.println(Printer.sprintf("[KNX] Received %i from %s, starting Scan Loop", targetValue, target));
        startScanLoop();
      }
    }
  }
}

void setup() {
  // Init Serial for print
  Serial.begin(115200);
  Serial.println("\n\n------------------------------------------------");
  Serial.println("[INFO] esp32-garage-beacons started");

  // Init Digital Pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  // Check configs
  if (!areVariablesValid()) {
    Serial.println("\n[ERROR] Variables 'BLE_ADDRESSES' and 'KNX_ADDRESSES' should have the same length");
  } else {
    // Init Serial2 for KNX
    Serial2.begin(19200, SERIAL_8E1);
    if (!START_SCANNING) {
      knx.uartReset();
    }
    for (int i = 0; i < sizeof(KNX_READ_ADDRESSES) / sizeof(KNX_READ_ADDRESSES[0]); i++) {
      knx.addListenGroupAddress(KNX_READ_ADDRESSES[i]);
    }

    // Init BLE scanner
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);  // Active scan uses more power, but get results faster
    pBLEScan->setInterval(2000);
    pBLEScan->setWindow(1999);  // Less or equal setInterval value

    if (START_SCANNING) {
      Serial.println("\n[INFO] Starting Scan Loop because of configuration");
      startScanLoop();
    }
  }
}

void loop() {
  if (digitalRead(PIN_BUTTON) == LOW) {
    Serial.println("\n[INFO] Starting Scan Loop because of button input");
    startScanLoop();
  }
}

void startScanLoop() {
  digitalWrite(PIN_LED, HIGH);
  for (int z = 0; z < TOTAL_SCANS; z++) {
    Serial.println(Printer.sprintf("\n[INFO] Start round of scans %i/%i", z + 1, TOTAL_SCANS));
    resetFoundAddresses();
    for (int i = 0; i < NUMBER_OF_SCANS; i++) {
      scan(i);
      if (allAddressesFound()) {
        break;
      }
    }
    Serial.println("\t[INFO] Scans done, sending zeros to KNX");
    sendZerosToKnx();
    Serial.println(Printer.sprintf("[INFO] Round of scans %i/%i done", z + 1, TOTAL_SCANS));
  }
  Serial2.begin(19200, SERIAL_8E1);
  knx.uartReset();
  digitalWrite(PIN_LED, LOW);
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
  Serial.println(Printer.sprintf("\t[INFO] Start scan %i/%i", idx + 1, NUMBER_OF_SCANS));
  BLEScanResults foundDevices = pBLEScan->start(SCAN_TIME, false);
  Serial.println(Printer.sprintf("\t[INFO] Scan %i/%i done! Devices found: %i", idx + 1, NUMBER_OF_SCANS, foundDevices.getCount()));
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
  for (int i = 0; i < TOTAL_ELEMENTS; i++) {
    if (!foundAddresses[i]) {
      Serial.println(Printer.sprintf("\t\t[KNX] Sent 0 to %s", KNX_ADDRESSES[i]));
      knx.groupWriteBool(KNX_ADDRESSES[i], false);
    }
  }
}
