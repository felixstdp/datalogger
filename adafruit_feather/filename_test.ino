#include "RTClib.h"

RTC_PCF8523 rtc;

void setup() {
  // put your setup code here, to run once:
  while (!Serial){;}
  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  
  if (! rtc.initialized() || rtc.lostPower()) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  DateTime now = rtc.now();

  char filename[16];
    
  sprintf(filename, "/%04d%02d%02dLOG.CSV", now.year(), now.month(), now.day());  

  //strcpy(filename,"Hello");

  Serial.println(filename);
  //Serial.flush();
}

void loop() {
  // put your main code here, to run repeatedly:

  
}
