/**
 * Program: ePaper-clock
 * Purpose:
 *   A basic clock display getting the time from the Internet. It's tailored for the 
 *   LilyGo T5 with 2.13" screen.
 * Notes:
 *   1) Uses the ESP32 Dev Module board definition in the Arduino IDE.
 *   2) It's necessary to install the GxEPD and Adafruit_GFX libraries
 * @author: David Argles, d.argles@gmx.com: drawing heavily on code by Lewis he
 */

/* Program identification */ 
#define PROG    "ePaper-clock"
#define VER     "1.0"
#define BUILD   "25Jun2021 @15:33h"

// Define the board (used later)
#define LILYGO_T5_V213

// Other defines
#define ORIENTATION 1 // 1 and 3 are landscape
#define SLEEPTIME   1 // in minutes

#include <boards.h>
#include <GxEPD.h>
#include <SD.h>
#include <FS.h>
#include <WiFi.h>
#include "time.h"

// Local includes
#include "flashscreen.h"
#include "speaker.h"
#include "myInfo.h"

// Not sure where this file is hiding :/
#include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13" b/w old panel

#include GxEPD_BitmapExamples

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

// Instantiate my local objects
flashscreen   flash;

// Global variables
const char* ntpServer = "pool.ntp.org"; // The Internet time server
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
struct tm   timeinfo;

// Needed to create the display instance below
GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
// display will be the class we use to manage the ePaper screen
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);

bool setupSDCard(void)
{
#if defined(_HAS_SDCARD_) && !defined(_USE_SHARED_SPI_BUS_)
    SPIClass SDSPI(VSPI);
#endif

#if defined(_HAS_SDCARD_) && !defined(_USE_SHARED_SPI_BUS_)
    SDSPI.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI);
    return SD.begin(SDCARD_CS, SDSPI);
#elif defined(_HAS_SDCARD_)
    return SD.begin(SDCARD_CS);
#endif
}
/*--------------------------------------------------------------
  -- We're expecting to deep sleep, so all the code goes here --
  --------------------------------------------------------------*/
void setup()
{
    Serial.begin(115200);
    // Send program details to serial output
    flash.message(PROG, VER, BUILD);
    
    // Start up the ePaper display
    SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
    display.init();
    display.setTextColor(GxEPD_BLACK);
    display.setRotation(ORIENTATION);

    testSpeaker();

    Serial.println("\nWiFi setup...");

    bool gotTime = false;
    if(connectWiFi()){
      if(getLocalTime()) gotTime = true;
    }

    WiFi.disconnect();    // Don't need this on any more
    WiFi.mode(WIFI_OFF);  // Save power

    if(gotTime){
      char timeStringBuff[50]; //50 chars should be enough
      strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
      //print like "const char*"
      display.println(timeStringBuff);
      changeFont("FreeMonoBold9pt7b", &FreeMonoBold9pt7b);
      display.fillScreen(GxEPD_WHITE);
      display.setCursor(0,20);
      //display.printf("Connecting to\n  %s \n", ssid);
      display.println(&timeinfo, "%A\n%B %d %Y\n%H:%Mh");
      display.update();
    }

    display.powerDown();

    Serial.println("\nPowering down...");
    // Set wake-up timer
    esp_sleep_enable_timer_wakeup(SLEEPTIME*60*1000000ULL);
    // Allow manual restart
    esp_sleep_enable_ext1_wakeup(((uint64_t)(((uint64_t)1) << BUTTON_1)), ESP_EXT1_WAKEUP_ALL_LOW);
    esp_deep_sleep_start();
}

void loop(){
  // Nothing happens here, we deep sleep instead
}
/*--------------------------------------------------------------
  ------------------- Helper functions follow ------------------
  --------------------------------------------------------------*/
void changeFont(const char name[], const GFXfont *f){
    display.setFont(f);
}

bool connectWiFi()
{
    bool success = false;
    Serial.printf("Connecting to\n  %s \n", ssid);
    WiFi.begin(ssid, password);
    int count = 5;
    while (WiFi.status() != WL_CONNECTED && count>0) {
      delay(500);
      Serial.print(".");
    }
    if(WiFi.status() == WL_CONNECTED){
      Serial.println("CONNECTED");
      success = true;
    }
    else Serial.println("Failed to connect");

    return (success);
}

bool getLocalTime()
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return false;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y @ %H:%Mh");
  return true;
}
