#include "RTClib.h"
unsigned long timestamp=millis();
#define LED_RED LED_BUILTIN

RTC_PCF8523 rtc;

void setup() {
  // put your setup code here, to run once:
  while (!Serial){;}
  Serial.begin(115200);

  // initialize Real Time Clock (RTC)
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
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis()-timestamp>=1000) // time between measurements in milliseconds
  {
    timestamp=millis();
    if (timestamp>=4294966796) timestamp-=4294966796;

    DateTime now = rtc.now();
    int windspeed=analogRead(A1); // values 0-1023 corresponding 0-3.3v

    // creates string to log onto SD card

    char logline[30];
    sprintf(logline, "%04d/%02d/%02d,%02d:%02d:%02d,%04u", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), windspeed);
    Serial.println(logline);
  }
  
}

// blink out an error code
void error(uint8_t errnum) {
  while(1) {
    uint8_t i;
    for (i=0; i<errnum; i++) {
      digitalWrite(LED_RED, HIGH);
      delay(100);
      digitalWrite(LED_RED, LOW);
      delay(100);
      yield();
    }
    for (i=errnum; i<10; i++) {
      delay(200);
      yield();
    }
  }
}
