// Adalogger FeatherWing data logger, log data on pin A0

#include <SPI.h>
#include <SD.h>

// Feather 32u4 basic proto
#define LED_RED LED_BUILTIN
#define SD_CS   10

File logfile;

void setup() {
  Serial.begin(115200);
  
  Serial.println("\r\nAnalog logger test");
  pinMode(LED_RED, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CS)) {
    Serial.println("Card init. failed!");
    error(2);
  }
  
  Serial.println("SD card OK");
  
  char filename[15];
  strcpy(filename, "/ANALOG00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[7] = '0' + i/10;
    filename[8] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

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
  digitalWrite(LED_RED, HIGH);  // LED onn during logging
  // Read the data on pin A0 and log it to SD card and to serial
  logfile.print("A0 = "); logfile.println(analogRead(0));
  Serial.print("A0 = "); Serial.println(analogRead(0));
  digitalWrite(LED_RED, LOW);  // LED off when not actively logging

  // save the output!
  logfile.flush();

  delay(100); // Change the delay to space out readings
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
