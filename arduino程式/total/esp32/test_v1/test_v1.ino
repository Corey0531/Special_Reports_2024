#include "BluetoothSerial.h"

BluetoothSerial SerialBT;  // 建立藍牙 Serial 物件
int currentMode = -1;      // 當前模式



// 定義 GPIO 腳位
const int pin4 = 4;          // 模式判別
const int pin5 = 5;          // 模式判別
const int encoderA = 12;     // 編碼器
const int encoderB = 13;     // 編碼器
const int AirIn = 25;        // 5/3 閥
const int AirOut = 26;       // 5/3 閥
const int FlexSensor1 = 34;  // 類比輸入（ESP32 使用 GPIO34 讀取類比訊號）

int counter = 0;  // 編碼器計數
int aState, aLastState = 0;
int data1 = 0;

unsigned long lastUpdateTime = 0;
const unsigned long interval = 500;

void setup() {
  pinMode(pin4, INPUT_PULLUP);
  pinMode(pin5, INPUT_PULLUP);
  pinMode(encoderA, INPUT);
  pinMode(encoderB, INPUT);
  pinMode(AirIn, OUTPUT);
  pinMode(AirOut, OUTPUT);

  Serial.begin(115200);
  SerialBT.begin("ESP32_Controller");  // 設定藍牙名稱
  Serial.println("ESP32 藍牙已啟動，等待連接...");
  aLastState = digitalRead(encoderA);
}

void loop() {
  int state4 = digitalRead(pin4);
  int state5 = digitalRead(pin5);
  int newMode = getMode(state4, state5);

  data1 = analogRead(FlexSensor1);  // 讀取類比感測器值

  unsigned long currentTime = millis();
  if (currentMode != newMode || currentTime - lastUpdateTime >= interval) {
    currentMode = newMode;
    lastUpdateTime = currentTime;
    executeMode(currentMode);
  }

  readEncoder();
}

void readEncoder() {
  aState = digitalRead(encoderA);
  if (aState != aLastState) {
    if (digitalRead(encoderB) != aState) {
      counter++;
    } else {
      counter--;
    }

    Serial.print("Position: ");
    Serial.println(counter);
  }
  aLastState = aState;
}

int getMode(int state4, int state5) {
  if (state4 == 1 && state5 == 1) return 0;
  if (state4 == 1 && state5 == 0) return 1;
  if (state4 == 0 && state5 == 1) return 2;
  if (state4 == 0 && state5 == 0) return 3;
  return -1;
}

void executeMode(int mode) {
  switch (mode) {
    case 0:
      Serial.println("模式 0：執行模式 0 的操作");
      Serial.println(data1);
      Serial.println(counter);
      break;
    case 1:
      Serial.println("模式 1：執行模式 1 的操作");
      digitalWrite(AirIn, HIGH);
      digitalWrite(AirOut, HIGH);
      break;
    case 2:
      Serial.println("模式 2：執行模式 2 的操作");
      digitalWrite(AirIn, HIGH);
      digitalWrite(AirOut, HIGH);
      break;
    case 3:
      Serial.println("模式 3：執行模式 3 的操作");
      Serial.println(data1);
      break;
    default:
      Serial.println("未知模式");
      break;
  }
}
