#include <SPI.h>
#include "RF24.h"

RF24 rf24(8, 7); // CE腳, CSN腳

const byte addr[] = "00001";
const byte pipe = 1;  // 指定通道編號

float msg[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,}; //共32byte 上限32byte
             //    h,  t1,  t2,  p1,  p2,   x,   y,   z
float Sp = 1000.0;

void setup() {
  Serial.begin(115200);
  while(!rf24.begin()){
    Serial.println("fail");
  }

  rf24.setChannel(83);  // 設定頻道編號
  rf24.setPALevel(RF24_PA_MIN);
  rf24.setDataRate(RF24_2MBPS);
  rf24.openReadingPipe(pipe, addr);  // 開啟通道和位址
  rf24.startListening();  // 開始監聽無線廣播
  Serial.println("nRF24L01 ready!");
}

void loop() {
  if (rf24.available(&pipe)) {
    rf24.read(&msg, sizeof(msg));
    for (auto &i : msg) {
      Serial.print(i);
      Serial.print(' ');
    }Serial.print(pToH(msg[3], Sp));
    Serial.print('\n');
  }
  delay(10);
}

float pToH(float p, float sp) {
  return 44330.0 * (1.0 - pow(p / sp, 0.1903));
}
