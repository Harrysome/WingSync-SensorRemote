#include <Streaming.h>
#include <Wire.h>

#include <DHT22.h>
const int data = 2;
DHT22 dht22(data);
float sp = 1000.0; //seaLevelPressure (hPa)

#include <Adafruit_MPL3115A2.h>
Adafruit_MPL3115A2 mpl;

#include "MS5611.h"
MS5611 MS5611(0x77);

#include <MPU6050.h>
MPU6050 mpu;
const int add = 0x68;   //the addr of MPU6050
const int sen = 0;   //the sensitivity from 0 ~ 6

#include <SPI.h>
#include "RF24.h"

RF24 rf24(8,9); // CE腳, CSN腳
const byte addr[] = "00001";
float msg[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; //共32byte 上限32byte

//#include <SD.h>
//File myFile;

float h, t1, t2, p1, p2;   // alt,t3;

void setup()
{
  pinMode(4,OUTPUT);
  pinMode(9,OUTPUT);
  
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
//  if (!SD.begin(4)) {
//    Serial.println("initialization failed!");
//    delay(100);
//  }Serial.println("----------------------------------");
  
  MS5611.begin();
  mpu.begin(add, sen);
  mpl.begin();
  mpl.setMode(MPL3115A2_BAROMETER);

  if(!rf24.begin()){
    Serial.println("Not responding");
    delay(100);
  }
  rf24.setChannel(83);       // 設定頻道編號
  rf24.openWritingPipe(addr); // 設定通道位址
  rf24.setPALevel(RF24_PA_MIN);   // 設定廣播功率
  rf24.setDataRate(RF24_2MBPS); // 設定傳輸速率
  rf24.stopListening();       // 停止偵聽；設定成發射模式
}

void loop()
{
  MS5611.setOversampling(OSR_ULTRA_HIGH);
  MS5611.read();
  p1 = MS5611.getPressure();    msg[3] = p1;
  t1 = MS5611.getTemperature(); msg[1] = t1;
  Serial << "T1 : " << t1 << " P1 : " << p1 << endl;

  mpl.startOneShot();
  t2 = mpl.getLastConversionResults(MPL3115A2_TEMPERATURE); msg[2] = t2;
  p2 = mpl.getLastConversionResults(MPL3115A2_PRESSURE);    msg[4] = p2;
  Serial << "T2 : " << t2 << " P2 : " << p2 << endl;

//  t3 = dht22.getTemperature(); msg[3] = t3;
  h = dht22.getHumidity();     msg[0] = h;
  Serial << "T3: " << dht22.getTemperature() << " °C, humidity: " << h << " %." << endl;

//  alt = pToH(p1, sp); msg[6] = alt; //Using ?? for pressure
//  Serial << "Altitude = " << alt << " meter." << endl;

  mpu.read();   //read all the num
  msg[5] = mpu.Xaxis();
  msg[6] = mpu.Yaxis();
  msg[7] = mpu.Zaxis();
  Serial << "( x, y, z) = ( " << mpu.Xaxis() << ", " << mpu.Yaxis() << ", " << mpu.Zaxis() << ")" << endl;

//  for(int i=0;i<8;i++){
//    Serial << msg[i] << ' ';
//  }Serial.println();

  rf24.write(&msg, sizeof(msg));  // 傳送資料

//  myFile = SD.open("data.txt", FILE_WRITE);
//  if (myFile) {                   // if the file opened okay, write to it:
//    //myFile.println("hum, temp1, temp2, temp3, press1, press2, alt");
//    myFile.print(h); myFile.print(' ');
//    myFile.print(t1); myFile.print(' ');
//    myFile.print(t2); myFile.print(' ');
//    myFile.print(t3); myFile.print(' ');
//    myFile.print(p1); myFile.print(' ');
//    myFile.print(p2); myFile.print(' ');
//    myFile.print(alt); myFile.println();
//    myFile.close();               // close the file:
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }
  
  delay(100);
}

float pToH(float p, float sp) {
  return 44330.0 * (1.0 - pow(p / sp, 0.1903));
}
