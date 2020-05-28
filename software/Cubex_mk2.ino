#include "SPI.h"
#include "SD.h"
#include "Wire.h"
#include "i2c.h"
#include "i2c_BMP280.h"
#include "MPU6050.h"
#include "EEPROM.h"

//---------------------------------------------------------------------------------------------------- PRESET
//----------------------------------------------------------------------------------------------------
//--------------------    SD Card     --------------------    ???>    zbieranie danych
void SDcard();
void SDcard_setup();
File myFile;
int sd=1, filenum;
char filename[8] = {'l','o','g','x','.','t','x','t'};
unsigned long time;
//--------------------    GY-BMP280   --------------------    ???>    altimetr
void altimetr();
BMP280 bmp280;
float temp, pascal, high;
//--------------------    MPU-6050    --------------------    ???>    zyroskop i akcelelometr
void pozycja();
MPU6050 mpu;
double Xalf, Yalf, Zalf, Xacc, Yacc, Zacc;
//--------------------    Led RGB     --------------------    ???>    dioda powiadomień
  #define Rled 3
  #define Gled 2
  #define Bled 4
  #define swich 5
//--------------------    Tested      --------------------    ???>    Test czujnikow
void test();
//---------------------------------------------------------------------------------------------------- SETUP
//----------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  Serial.println("Seraial Start");
  time = millis();
// LED
  pinMode(Rled, OUTPUT);
  pinMode(Gled, OUTPUT);
  pinMode(Bled, OUTPUT);
// test
  test();
// SD card
  SDcard_setup();

}
//---------------------------------------------------------------------------------------------------- LOOP
//----------------------------------------------------------------------------------------------------

void loop() {
  if(!digitalRead(swich)){
    altimetr();
    pozycja();
    SDcard();
    delay(100);
    time = millis();
  }
}
//---------------------------------------------------------------------------------------------------- SNENSORS
//----------------------------------------------------------------------------------------------------

void altimetr(){   
  bmp280.awaitMeasurement();
  bmp280.getTemperature(temp);
  bmp280.getPressure(pascal);
    float sealevelPressure = 101325;
  high = 44330 * (1.0 - pow(pascal / sealevelPressure, (1/5.255)));
  bmp280.triggerMeasurement();
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void pozycja(){
  Vector rawGyro = mpu.readRawGyro();
  Vector rawAccel = mpu.readRawAccel();
  
  Xalf = rawGyro.XAxis;
  Yalf = rawGyro.YAxis;
  Zalf = rawGyro.ZAxis;
  Xacc = rawAccel.XAxis;
  Yacc = rawAccel.YAxis;
  Zacc = rawAccel.ZAxis;
}
//---------------------------------------------------------------------------------------------------- SDCARD
//----------------------------------------------------------------------------------------------------

void SDcard(){
  digitalWrite(Bled,true);
  myFile = SD.open(filename, FILE_WRITE);
  myFile.print(sd);     myFile.print(";");
  myFile.print(time);   myFile.print(";");
// altimetr data
  myFile.print(temp);   myFile.print(";");
  myFile.print(pascal); myFile.print(";");
  myFile.print(high);   myFile.print(";");
// pozycja data
  myFile.print(Xalf);     myFile.print(";");
  myFile.print(Yalf);     myFile.print(";");
  myFile.print(Zalf);     myFile.print(";");
  myFile.print(Xacc);     myFile.print(";");
  myFile.print(Yacc);     myFile.print(";");
  myFile.print(Zacc);     myFile.print(";");
//end record
  myFile.println();
  myFile.close();
  sd++;
  digitalWrite(Bled,false);
};
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void SDcard_setup(){
//EEPROM
  filenum = EEPROM.read(0);
  EEPROM.write(0, filenum+1);
  if(filenum>=9)EEPROM.write(0, 0);
  filename[3] = '0'+EEPROM.read(0);
  
//Creating file
  Serial.println(filename);
  myFile = SD.open(filename, FILE_WRITE);
  myFile.close();
  myFile = SD.open(filename, FILE_WRITE);
  myFile.print("Lp\ttime\ttemp\tpascal\thigh\tXalf\tYalf\tZalf\tXacc\tYacc\tZacc");
  myFile.println("======================================================================");
  myFile.close();
  
}
//---------------------------------------------------------------------------------------------------- TEST
//----------------------------------------------------------------------------------------------------

void test(){
// test 1
  Serial.println("Start Test");
// LED test
  Serial.println("LED test");
  digitalWrite(Rled, true); delay(200); digitalWrite(Rled, false); 
  digitalWrite(Gled, true); delay(200); digitalWrite(Gled, false); 
  digitalWrite(Bled, true); delay(200); digitalWrite(Bled, false); 
//
  digitalWrite(Bled,true);
  delay(500);
  digitalWrite(Bled,false);
//SD card test
  Serial.print("SD test"); 
  while (!SD.begin(10)) {
    digitalWrite(Rled,true);
    delay(250);
    digitalWrite(Rled,false);
    delay(250);  
  }
  if(SD.begin(10)){
    for(int i=0; i<3; i++){
      digitalWrite(Gled,true);
      delay(250);
      digitalWrite(Gled,false);
      delay(250);
    }}
  Serial.println(" done");
  
// test 2
  digitalWrite(Bled,true);
  delay(500);
  digitalWrite(Bled,false);
  
//BMP280 tset
  Serial.print("BMP test");
  while(!bmp280.initialize()){
      digitalWrite(Rled,true);
      delay(250);
      digitalWrite(Rled,false);
      delay(250);
  }
  if(bmp280.initialize()){
    for(int i=0; i<3; i++){
      digitalWrite(Gled,true);
      delay(250);
      digitalWrite(Gled,false);
      delay(250);
    }}
  bmp280.setEnabled(0);
  bmp280.triggerMeasurement();
  Serial.println(" done");
  
// test 3
  digitalWrite(Bled,true);
  delay(500);
  digitalWrite(Bled,false);
  
//MPU-6050 tset
  Serial.print("MPU test");
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)){
  digitalWrite(Rled,true);
  delay(250);
  digitalWrite(Rled,false);
  delay(250);
  }
  if(mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)){
    for(int i=0; i<3; i++){
      digitalWrite(Gled,true);
      delay(250);
      digitalWrite(Gled,false);
      delay(250);
    }}
  mpu.calibrateGyro();
  mpu.setThreshold(3);
  Serial.println(" done");
  
// End of test
  digitalWrite(Rled,true);
  digitalWrite(Gled,true);
  digitalWrite(Bled,true);
  delay(1000);
  digitalWrite(Rled,false);
  digitalWrite(Gled,false);
  digitalWrite(Bled,false);
}
