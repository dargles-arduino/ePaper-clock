# ePaper-clock
A simple clock designed to work with the LilyGo ESP32 T5 plus the 2.13" ePaper display. Since it only sleeps for approximately 52 seconds, I'm not sure how useful it really is - I haven't done any power consumption tests. But the main idea is to get a working project that implements the main parts of a typical ePaper project.

![ePaper-clock](https://user-images.githubusercontent.com/5638741/123490736-bd5ef280-d60c-11eb-94fe-85b94f46dde8.jpg)

It _should_ all work if you:

  - set the board driver to be the ESP32 dev module;
  - make sure the Adafruit_GFX and
  - the GxEPD libraries are installed.

In addition, the code refers to a "myInfo.h" file which you'll have to create. This contains just two lines as follows:

   const char* ssid       = "mySSID";
   
   const char* password   = "myPassword";
   
Or you could just include these two lines in the main code and delete the line that includes myInfo.h. I do things this way so I don't expose my personal WiFi details on GitHub.

The code works as follows:

  - It checks for, and sets up any SD card that might be inserted.
  - It checks for a speaker.
  - It then connects to the local WiFi...
  - ...and contacts the Internet time server to get the current time.
  - It then swirthces off the WiFi to save a bit of power,
  - clears the screen and prints out the current time,
  - works out how long to wait until the next minute roll-over, and 
  - goes into deep sleep for that many seconds (about 52 on average).
  
Because of the way it works, it usually updates the time about 8 seconds after Internet time actually rolls over.

_David Argles 25th June 2021_

