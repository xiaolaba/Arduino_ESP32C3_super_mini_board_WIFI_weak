/// ESP32-C3/S3_super-mini
/// 4M flash, 2M PSRAM(S3 only)
/// USB CDC on boot : enabled
/// flash mode : DIO
/// testing done, ok, by xiao_laba, 2022

#include <WiFi.h>
#include "time.h"
#include "esp_sntp.h"

//const char *ssid = "YOUR_SSID";
//const char *password = "YOUR_PASS";
//const char *ssid = "my wifi";
//const char *password = "12345678";
//#include "my_wifi.hpp"

const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

// see https://github.com/....../posix_tz_db/blob/master/zones.csv
// see https://github.com/....../blob/master/cores/esp8266/TZ.h
// Asia/Taipei  CST-8
// Asia/Tokyo  JST-9
// Asia/Bangkok  <+07>-7
// Europe/Amsterdam  CET-1CEST,M3.5.0,M10.5.0/3
//const char *time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)
//const char location = "Europe/Amsterdam CET-1CEST,M3.5.0,M10.5.0/3";  // location
const char *time_zone = "CST-8";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)
const char *location = "Asia/Taipei CST-8";  // location

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  Serial.println(location);
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

// Callback function (gets called when time adjusts via NTP)
void timeavailable(struct timeval *t) {
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

void setup() {
  //// CPU : ESP32-D0WD-V3 (revision v3.1), QFN5x5
  //// WeMos D1_R32 or clone, 4MB SPI external flash storage
  //// ESP32-C3 (super-mini,) clone
  //// CPU : ESP32-C3 (QFN32) (revision v0.4)
  //// Features: Wi-Fi, BT 5 (LE), Single Core, 160MHz, Embedded Flash 4MB (XMC)
  //// USB CDC is used, no USB-UART chip is used.
  //// ESP32-S3 (super-mini), clone
  //// CPU : ESP32-S3
  //// 4MB FLASH, 2MB PSRAM
  //// USB CDC is used, no USB-UART chip is used.

  delay(6000);
  //Serial.begin(500000); // no match boot log speed
  Serial.begin(115200); // match boot log speed, 
  Serial.print("\nTesting WIFI ESP32 & time NTP by xiaolaba\n");
  Serial.print("Compiled on: ");
  Serial.print(__DATE__);
  Serial.print(" at ");
  Serial.println(__TIME__);
  Serial.printf("\nESP32 Chip model = %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("This chip has %d cores\n", ESP.getChipCores());
  uint32_t flashSize = ESP.getFlashChipSize();
  Serial.printf("Flash Size: %u bytes\n", flashSize);
  uint32_t psramSize = ESP.getPsramSize();
  Serial.printf("PSRAM Size: %u bytes\n", psramSize);
  uint32_t freeMemory = ESP.getFreeHeap();
  Serial.printf("Free Memory: %u bytes\n", freeMemory);
  uint32_t Totalheap = ESP.getHeapSize();
  Serial.printf("Total heap: %u bytes\n", Totalheap);
  uint32_t FreePSRAM = ESP.getFreePsram();
  Serial.printf("Free PSRAM: %u bytes\n", FreePSRAM);
  // First step is to configure WiFi STA and connect in order to get the current time and date.
  //Serial.printf("Connecting to %s\n", ssid);
  Serial.printf("Connecting to my wifi..\n");
  WiFi.begin(ssid, password);

  /**
   * NTP server address could be acquired via DHCP,
   *
   * NOTE: This call should be made BEFORE esp32 acquires IP address via DHCP,
   * otherwise SNTP option 42 would be rejected by default.
   * NOTE: configTime() function call if made AFTER DHCP-client run
   * will OVERRIDE acquired NTP server address
   */
  esp_sntp_servermode_dhcp(1);  // (optional)
  while (WiFi.status() != WL_CONNECTED) {
    for (uint8_t i = 0; i < 40; i++) { 
      delay(500);
      Serial.print(".");
    }
    Serial.print("\n");
  }
  Serial.println(" CONNECTED");

  // set notification call-back function
  sntp_set_time_sync_notification_cb(timeavailable);
  /**
   * This will set configured ntp servers and constant TimeZone/daylightOffset
   * should be OK if your time zone does not need to adjust daylightOffset twice a year,
   * in such a case time adjustment won't be handled automagically.
   */
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  /**
   * A more convenient approach to handle TimeZones with daylightOffset
   * would be to specify a environment variable with TimeZone definition including daylight adjustmnet rules.
   * A list of rules for your zone could be obtained from https://github.com/....../blob/master/cores/esp8266/TZ.h
   */
  configTzTime(time_zone, ntpServer1, ntpServer2);
}



/*
RSSI Value Range  WiFi Signal Strength
RSSI > - 30 dBm  Amazing
RSSI < – 55 dBm  Very good signal
RSSI < – 67 dBm  Fairly Good
RSSI < – 70 dBm  Okay
RSSI < – 80 dBm  Not good
RSSI < – 90 dBm  Extremely weak signal (unusable)
*/
int8_t rssi = 0;

void loop() {
  delay(2000);
  rssi = WiFi.RSSI();
  Serial.printf("WiFi Received Signal Strength Indicator (RSSI) %4d dbm, ", rssi);
  if (rssi > -30) {
    Serial.printf("Amazing\n\n");
  } else if (rssi >= -55) {
    Serial.printf("very good\n\n");
  } else if (rssi >= -67) {
    Serial.printf("fairly good\n\n");
  } else if (rssi >= -70) {
    Serial.printf("ok\n\n");
  } else if (rssi >= -80) {
    Serial.printf("no good\n\n");
  } else if (rssi >= -90) {
    Serial.printf("weak and unstable\n");
  } else {
    Serial.printf("very weak, unusable\n");
  }
  printLocalTime();  // it will take some time to sync time 🙂
}

