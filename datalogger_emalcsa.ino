#include <SD.h>
#include <Wire.h>

#define clock 0x68
#define chipSelect 10
#define valvula1 8
#define valvula2 9

unsigned long timestamp=millis();
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
char dataString[32];
int operacion = 0;

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
}

void loop()
{
// lee el reloj externo (RTC)

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

// operaciones con valvulas 

  if (operacion==0) {
    digitalWrite(valvula1,HIGH);  // minuto 0 abrir
    digitalWrite(valvula2,HIGH);
  }

  if (operacion==10) {
    digitalWrite(valvula1,LOW);  // minuto 10 cerrar
    digitalWrite(valvula2,LOW);
  }

// lectura de las células (suma de 30 medidas)

  unsigned long celula0=0;
  unsigned long celula1=0;
  
  for (int i=0;i<30;i++){
    celula0 += analogRead(0);
    celula1 += analogRead(1);
    delay(1000);
  }

  celula0 = celula0/30; //promedio de 30 medidas
  celula1 = celula1/30;

// código pendiente: convertir V a unidades físicas

// ...

// escribe los datos a una cadena de caracteres

  sprintf(dataString, "%02d/%02d/20%02d;%02d:%02d:%02d;%04u;%04u", dayOfMonth, month, year, hour,minute,second,celula0, celula1);	

// muestra los datos por el puerto serie
    
  Serial.println(dataString);
  
// graba los datos en la tarjeta SD

  File dataFile = SD.open("data.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  else {
    Serial.println("error opening data.txt");
  }

// espera 10 minutos hasta la próxima operación

  while (millis()-timestamp<600000){}

  timestamp=millis();
  if (timestamp>=4294966796) timestamp-=4294966796; // pone el contador millis a cero (duración 50 días)

  operacion += 10;
  if (operacion==30) operacion=0;  // cambia el contador de operacion

}

byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}