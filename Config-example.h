// Example Configuration File
// This file is an example, you should change the configuration parameters you want and rename it to Config.h

const int TOTAL_SECONDS_SCANNING = 60;             // Time doing scans when KNX signal is received (in seconds)
const int NUMBER_OF_SCANS = 3;                     // Number of retries if device is not found (to prevent false negatives)
const int SCAN_TIME = 5;                           // Time of each scan (in seconds)
const bool START_SCANNING = true;                  // If true, the first scan doesn't need a KNX signal, it starts right away
String BLE_ADDRESSES[] = { "XX:XX:XX:XX:XX:XX" };  // BLE devices addresses
String KNX_ADDRESSES[] = { "X/X/X" };              // KNX addresses to write when devices are found
String KNX_READ_ADDRESSES[] = { "Y/Y/Y" };         // KNX addresses to listen in order to start scanning