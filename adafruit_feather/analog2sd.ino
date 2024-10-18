#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
// Feather 32u4 basic proto
#define LED_RED LED_BUILTIN
#define SD_CS   10

RTC_PCF8523 rtc;
File logfile;

unsigned long timestamp=millis();

void setup() {
  // put your setup code here, to run once:
  while (!Serial){;}
  Serial.begin(115200);

  pinMode(LED_RED, OUTPUT);

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

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CS)) {
    Serial.println("Card init. failed!");
    error(2);
  }
  Serial.println("SD card OK");

  // naming the log.csv file to save in the SD card

  DateTime now = rtc.now();
  char filename[12];
  sprintf(filename, "/%04d%02d%02d.CSV", now.year(), now.month(), now.day());  
    
  Serial.println(filename);

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    error(3);
  }
  Serial.print("Writing to "); 
  Serial.println(filename);
  Serial.println("Ready!");

}

void loop() {
  // put your main code here, to run repeatedly:

  // ----------------------------------
  //        IMPORTANT!
  // ----------------------------------
  
  if (millis()-timestamp>=1000) // time between measurements in milliseconds
  {
    timestamp=millis();
    if (timestamp>=4294966796) timestamp-=4294966796;

    DateTime now = rtc.now();
    
    int U=analogRead(A0); // values 0-1023 corresponding 0-3.3v-5v
    int V=analogRead(A1); 
    int W=analogRead(A2); 
    int S=analogRead(A3); // status

    // creates string to log onto SD card

    char logline[40];
    sprintf(logline, "%04d/%02d/%02d,%02d:%02d:%02d,%04u,%04u,%04u,%04u", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), U, V, W, S);
    Serial.println(logline);
    logfile.println(logline);
    // save the output!
    logfile.flush();
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
