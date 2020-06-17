#include <SD.h>
#include <Wire.h>

#define clock 0x68
#define chipSelect 10
#define valvula1 8
#define valvula2 9

unsigned long timestamp=millis();
unsigned long entrada0, entrada1, entrada2;
float caudal, presion1, presion2;
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
char dataString[32];
int operacion = 0;

void setup()
{
  pinMode(valvula1,OUTPUT);
  pinMode(valvula2,OUTPUT);
  digitalWrite(valvula1,HIGH);
  digitalWrite(valvula2,HIGH);
  
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
    digitalWrite(valvula1,LOW);  // minuto 0 abrir
    digitalWrite(valvula2,LOW);
  }

  if (operacion==10) {
    digitalWrite(valvula1,HIGH);  // minuto 10 cerrar
    digitalWrite(valvula2,HIGH);
  }

  if (operacion==20) {
    digitalWrite(valvula1,HIGH);  // minuto 20 cerrar
    digitalWrite(valvula2,HIGH);
  }
  
// lectura de las células (suma de 30 medidas)

  entrada0=0;
  entrada1=0;
  entrada2=0;
  
  for (int i=0;i<30;i++){
    entrada0 += analogRead(A0);
    entrada1 += analogRead(A1);
    delay(10);
  }

  entrada0 = entrada0/30; //promedio de 30 medidas
  entrada1 = entrada1/30;
  entrada2 = entrada2/30;

// código pendiente: convertir V a unidades físicas

  caudal = entrada0;
  presion1 = entrada1 *5 /1023;
  presion2 = entrada2 *5 /1023;
    

// escribe los datos a una cadena de caracteres

  sprintf(dataString, "%02d/%02d/20%02d;%02d:%02d:%02d;%d.%02d;%d.%02d;%d.%02d", dayOfMonth, month, year, hour,minute,second,(int)caudal, (int)(caudal*100)%100, (int)presion1, (int)(presion1*100)%100, (int)presion2, (int)(presion2*100)%100);	

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

  while ((millis()-timestamp)<6000){}

  timestamp=millis();
  if (timestamp>=4294966796) timestamp-=4294966796; // pone el contador millis a cero (duración 50 días)

  operacion += 10;
  if (operacion==30) operacion=0;  // cambia el contador de operacion

}

byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}
