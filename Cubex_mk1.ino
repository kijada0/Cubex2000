#include "SPI.h"
#include "SD.h"
#include "Wire.h"
#include "i2c.h"
#include "i2c_BMP280.h"
#include "MPU6050.h"
#include "EEPROM.h"

//--------------------    SD Card     --------------------    ???>    zbieranie danych
void SDcard();
File myFile;
int sd=1, numS, numD, numJ;
char filename[10] = {'l','o','g','0','0','0','.','t','x','t'};
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
//----------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  Serial.println("Seraial Start");
  time = millis();
// LED
  pinMode(Rled, OUTPUT);
  pinMode(Gled, OUTPUT);
  pinMode(Bled, OUTPUT);
//EEPROM
  numS = EEPROM.read(0);
    numD = EEPROM.read(1);
      numJ = EEPROM.read(2);
  EEPROM.write(2, numJ++);
  if(numS>=9&&numD>=9&&numJ>=9){EEPROM.write(0, 0); EEPROM.write(1, 0); EEPROM.write(2, 0);}
    if(numD>=9&&numJ>=9){EEPROM.write(0, numS++); EEPROM.write(1, 0); EEPROM.write(2, 0);}
      if(numS>=9){EEPROM.write(1, numD++); EEPROM.write(2, 0);}

// test
  test();
// SD card
  Serial.println(filename);
  myFile = SD.open(filename, FILE_WRITE);
  myFile.close();
  myFile = SD.open(filename);
  myFile.print("Lp\ttime\ttemp\tpascal\thigh\tXalf\tYalf\tZalf\tXacc\tYacc\tZacc");
  myFile.println("======================================================================");
  myFile.close();

}

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

//----------------------------------------------------------------------------------------------------
void SDcard(){
  digitalWrite(Bled,true);
  myFile = SD.open(filename);
  myFile.print(sd);     myFile.print("\t");
  myFile.print(time);   myFile.print("\t");
// altimetr data
  myFile.print(temp);   myFile.print(",\t");
  myFile.print(pascal); myFile.print(",\t");
  myFile.print(high);   myFile.print(",\t");
// pozycja data
  myFile.print("");     myFile.print(",\t");
  myFile.print("");     myFile.print(",\t");
  myFile.print("");     myFile.print(",\t");
  myFile.print("");     myFile.print(",\t");
  myFile.print("");     myFile.print(",\t");
  myFile.print("");     myFile.print(",\t");
//end record
  myFile.println();
  myFile.close();
  sd++;
  digitalWrite(Bled,false);
};

//----------------------------------------------------------------------------------------------------
void test(){
// test 1
  Serial.println("Start Test");
// LED test
  Serial.println("LED test");
  digitalWrite(Rled, true); delay(250); digitalWrite(Rled, false); 
  digitalWrite(Gled, true); delay(250); digitalWrite(Gled, false); 
  digitalWrite(Bled, true); delay(250); digitalWrite(Bled, false); 
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
  
// End test
  digitalWrite(Rled,true);
  digitalWrite(Gled,true);
  digitalWrite(Bled,true);
  delay(1000);
  digitalWrite(Rled,false);
  digitalWrite(Gled,false);
  digitalWrite(Bled,false);
}
