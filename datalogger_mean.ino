#include <SD.h>
#include <Wire.h>

#define DS1307_I2C_ADDRESS 0x68  // This is the I2C address
#define CHIP_SELECT 10
#define LED_PIN 7
#define PULSADOR_PIN 9 
 
// Parametros del datalogger (tiempo de registro)
byte segundos_record = 1;
byte ciclo_record = 0; // Para indicar los segundos que quedan para realizar grabación
boolean estado_pulsador = true;
byte operate_button = 0;
long previousMillis = 0;        // will store last time system was updated
int DATOS[4][5];
double MEDIA_DATOS[4]={0,0,0,0};
byte pos_Datos=9; // referencia para la lista de datos
byte contador_segundos=0;
unsigned int celula0, celula1;
char buffer[150];
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

/***************************  CONFIGURACION  ************************/

void setup() {
  Serial.begin(57600);
 
  if (!SD.begin(CHIP_SELECT))
      {Serial.println("FALLO SD...");
      blink_S(20);
      estado_pulsador=false;
    }
  else Serial.println("SD OK...");
  digitalWrite(LED_PIN, LOW);
  delay(500);
  analogReference(EXTERNAL);
  delay(100);
 
  if (estado_pulsador) digitalWrite(LED_PIN, HIGH);

  Wire.begin();

/*  second     = 0;
  minute     = 41;
  hour       = 19;  
  dayOfWeek  = 0;     Cuando se quiere actualizar la hora se establecen los valores y se quitan comentarios
  dayOfMonth = 10;
  month      = 12;
  year       = 12;*/

getDateDs1307();

}

/***************************  PRINCIPAL  ************************/

void loop() {

  if(millis() - previousMillis >= 500) {
    previousMillis = millis();   
    celula0=analogRead(0);
    celula1=analogRead(1);
    save_SD();
  }
}

/**************************** The LOOP End **********************/
void save_SD()
{
   File dataFile = SD.open("registro.txt", FILE_WRITE);
   if (dataFile) {
           dtostrf(MEDIA_DATOS[0],6,1, buffer);
           buffer[6]=';';
           buffer[7]=' ';
           dtostrf(MEDIA_DATOS[1],6,1, &buffer[8]);
           buffer[14]=';';
           buffer[15]=' ';
           sprintf(&buffer[16], "%02d:%02d:%02d; %02d/%02d/20%02d; \n", hour,minute,second, dayOfMonth, month, year);
   
           dataFile.println(buffer);
           dataFile.close();
           digitalWrite(STATUS_LED_PIN, HIGH);
         }  
    else
        {
          digitalWrite(STATUS_LED_PIN, LOW);
        }
}

/************************** Hacer blink en LedSTATUS ACTIVITY (internal LED in board) ****************************************/
void blink_S (int duracion)
{
  int i;
for (i=0; i<duracion; i++)
  {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(100); 
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(100); 
  }
}


// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}
 
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}

void setDate()                
{
 
   Wire.beginTransmission(DS1307_I2C_ADDRESS);
   Wire.write((byte)0x00);
   I2C_WRITE(decToBcd(second) & 0x7f);    // 0 to bit 7 starts the clock
   I2C_WRITE(decToBcd(minute));
   I2C_WRITE(decToBcd(hour));      // If you want 12 hour am/pm you need to set
                                   // bit 6 (also need to change readDateDs1307)
   I2C_WRITE(decToBcd(dayOfWeek));
   I2C_WRITE(decToBcd(dayOfMonth));
   I2C_WRITE(decToBcd(month));
   I2C_WRITE(decToBcd(year));
   Wire.endTransmission();
}
 
// Gets the date and time from the ds1307 and prints result
void getDate()
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write((byte)0x00);
  Wire.endTransmission();
 
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
 
  second     = bcdToDec(I2C_READ() & 0x7f);
  minute     = bcdToDec(I2C_READ());
  hour       = bcdToDec(I2C_READ() & 0x3f);
  dayOfWeek  = bcdToDec(I2C_READ());
  dayOfMonth = bcdToDec(I2C_READ());
  month      = bcdToDec(I2C_READ());
  year       = bcdToDec(I2C_READ());
}
