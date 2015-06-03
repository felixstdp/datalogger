#include <SD.h>
#include <Wire.h>

#define clock 0x68
#define chipSelect 8

unsigned long timestamp=millis();
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
char dataString[32];

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Probando tarjeta SD");
  pinMode(10, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Fallo en tarjeta");
  }
  else Serial.println("Tarjeta OK");
  analogReference(EXTERNAL);
}

void loop()
{
// lee el reloj externo

  Wire.beginTransmission(0x68);
  Wire.write((byte)0x00);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 7);
  second     = bcdToDec(Wire.read() & 0x7f);
  minute     = bcdToDec(Wire.read());
  hour       = bcdToDec(Wire.read() & 0x3f);  
  dayOfWeek  = bcdToDec(Wire.read());
  dayOfMonth = bcdToDec(Wire.read());
  month      = bcdToDec(Wire.read());
  year       = bcdToDec(Wire.read());

// lee las células

  int celula0=analogRead(0);
  int celula1=analogRead(1);

// escribe los datos a una cadena de caracteres

  sprintf(dataString, "%02d/%02d/20%02d;%02d:%02d:%02d;%04u;%04u", dayOfMonth, month, year, hour,minute,second, celula0, celula1);	

// graba los datos en la tarjeta SD cada 250 milisegundos

  if (millis()-timestamp>=250)
  {
    timestamp=millis();
    if (timestamp>=4294966796) timestamp-=4294966796;
    
    Serial.println(dataString);

    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
    }  
    else {
//      Serial.println("error de escritura en SD");
    }
  }
}

byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}
