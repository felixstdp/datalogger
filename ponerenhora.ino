#include <Wire.h>

#define DS1307_I2C_ADDRESS 0x68
#define CHIP_SELECT 10
 
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

void setup() {

  pinMode(CHIP_SELECT, OUTPUT);
  Serial.begin(9600);
 
  Wire.begin();

  second     = 0;
  minute     = 16;
  hour       = 10;  
  dayOfWeek  = 5; 
  dayOfMonth = 11;
  month      = 7;
  year       = 14;

  setDateDs1307();

}
 
void loop() {
  getDateDs1307();
  
  Serial.print(dayOfMonth);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/");
  Serial.print(year);

  Serial.print(" ");

  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);
  
  delay(1000);
}

byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}
 
byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}

void setDateDs1307()                
{
   Wire.beginTransmission(DS1307_I2C_ADDRESS);
   Wire.write(0x00);
   Wire.write(decToBcd(second) & 0x7f); 
   Wire.write(decToBcd(minute));
   Wire.write(decToBcd(hour));    
   Wire.write(decToBcd(dayOfWeek));
   Wire.write(decToBcd(dayOfMonth));
   Wire.write(decToBcd(month));
   Wire.write(decToBcd(year));
   Wire.endTransmission();
}
 
void getDateDs1307()
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
 
  second     = bcdToDec(Wire.read() & 0x7f);
  minute     = bcdToDec(Wire.read());
  hour       = bcdToDec(Wire.read() & 0x3f); 
  dayOfWeek  = bcdToDec(Wire.read());
  dayOfMonth = bcdToDec(Wire.read());
  month      = bcdToDec(Wire.read());
  year       = bcdToDec(Wire.read());
}


