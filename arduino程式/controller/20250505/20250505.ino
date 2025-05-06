#include "Calibration.h"
#include "BangBangControl.h"
//腳位定義
const int encoderPinA = 12, encoderPinB = 13;  // 編碼器腳位
const int AirIn = 8;                           //輸出調整五口三位閥
const int AirOut = 9;                          //輸出調整五口三位閥

//常數定義
volatile int counter = 0;  // 編碼器計數
int aState, aLastState = 0;
int data1 = 0;
const int encoderMax = 4;
const int encoderMin = 0;
//millis參數
unsigned long lastControlTime = 0;
const unsigned long controlInterval = 100;  // 控制頻率間隔（毫秒）
// 記錄每段的充氣時間（毫秒）
unsigned long inflateTimeTable[4] = { 0 };  // 0→1, 1→2, ..., 3→4

bool isCalibrated = false;
int targetPos = 0;
unsigned long inflateStartTime = 0;
bool inflating = false;

//編碼器程式，只有0-4
void IRAM_ATTR readEncoder() {
  aState = digitalRead(encoderPinA);
  if (aState != aLastState) {
    if (digitalRead(encoderPinB) != aState) {
      counter++;
    } else {
      counter--;
    }
    //測試讀取數值用
    Serial.print("Position: ");
    Serial.println(counter);
  }
  aLastState = aState;
}

void setup() {
  Serial.begin(115200);
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  pinMode(AirIn, OUTPUT);
  pinMode(AirOut, OUTPUT);

  aLastState = digitalRead(encoderPinA);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), readEncoder, CHANGE);

  digitalWrite(AirIn, LOW);
  digitalWrite(AirOut, LOW);
}

void loop() {
  unsigned long now = millis();

  if (now - lastControlTime >= controlInterval) {
    lastControlTime = now;

    int position;
    noInterrupts();
    position = counter;
    interrupts();

    bangBangWithTimingControl(position);

    Serial.print("Position: ");
    Serial.println(position);
  }
}

