#include <Arduino.h>

// ==== PIN 設定 ====
const int encoderPinA = 12;
const int encoderPinB = 13;
const int pin4 = 4;  // 模式切換
const int pin5 = 5;
const int AirIn = 25;
const int AirOut = 26;
const int FlexSensor1 = 34;  //類比輸入(ESP32使用GPI034 讀取類比訊號)

// ==== 全域變數 ====
volatile int counter = 0;
int aState, aLastState = 0;
int data1 = 0;
const int encoderMax = 5;
const int encoderMin = 0;

unsigned long lastControlTime = 0;
const unsigned long controlInterval = 100;
unsigned long inflateTimeTable[5] = { 0 };
bool isCalibrated = false;
int targetPos = 0;

int currentMode = -1;
unsigned long lastUpdateTime = 0;
const unsigned long modeInterval = 500;

// ==== 副程式定義 ====
void IRAM_ATTR readEncoder();
void calibrateInflationTimes();
void bangBangWithTimingControl(int currentPos, int targetPos);
int getMode();
void executeMode(int mode);
void executeMode(int mode);

void setup() {
  Serial.begin(115200);
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  pinMode(pin4, INPUT_PULLUP);
  pinMode(pin5, INPUT_PULLUP);
  pinMode(FlexSensor1, INPUT);
  pinMode(AirIn, OUTPUT);
  pinMode(AirOut, OUTPUT);

  aLastState = digitalRead(encoderPinA);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), readEncoder, CHANGE);

  digitalWrite(AirIn, LOW);
  digitalWrite(AirOut, LOW);
}

void loop() {
  unsigned long now = millis();
  int newMode = getMode();

  if (currentMode != newMode || now - lastUpdateTime >= modeInterval) {
    currentMode = newMode;
    lastUpdateTime = now;
    executeMode(currentMode);
  }
}



// ==== 編碼器中斷 ====
void IRAM_ATTR readEncoder() {
  aState = digitalRead(encoderPinA);
  if (aState != aLastState) {
    if (digitalRead(encoderPinB) != aState) {
      counter++;
    } else {
      counter--;
    }
    counter = constrain(counter, encoderMin, encoderMax);
    Serial.print("Position: ");
    Serial.println(counter);
  }
  aLastState = aState;
}

// ==== 校準充氣時間 ====
void calibrateInflationTimes() {
  Serial.println("\n[模式0] 開始自動校準...");
  counter = 0;
  for (int i = 0; i < 6; i++) {
    unsigned long startTime = millis();
    unsigned long timeout = millis();
    while (counter <= i) {
      if (millis() - timeout > 10000) {
        Serial.println("校準超時");
        break;
      }
      digitalWrite(AirIn, HIGH);
      digitalWrite(AirOut, LOW);
    }
    digitalWrite(AirIn, LOW);
    digitalWrite(AirOut, LOW);
    inflateTimeTable[i] = millis() - startTime;
    Serial.print("Inflate time ");
    Serial.print(i);
    Serial.print(" → ");
    Serial.print(i + 1);
    Serial.print(" = ");
    Serial.print(inflateTimeTable[i]);
    Serial.println(" ms");
    delay(500);
  }
  isCalibrated = true;
  Serial.println("校準完成");
}

// ==== 控制器 ====
void bangBangWithTimingControl(int currentPos, int targetPos) {
  static unsigned long inflateStart = 0;
  static bool inflating = false;

  if (!inflating && currentPos < targetPos && targetPos <= 5) {
    unsigned long totalTime = 0;
    for (int i = currentPos; i < targetPos; i++) {
      totalTime += inflateTimeTable[i];
    }
    inflateStart = millis();
    inflating = true;
    digitalWrite(AirIn, HIGH);
    digitalWrite(AirOut, LOW);
    Serial.println("啟動充氣");
  }

  if (inflating && millis() - inflateStart >= 1) {
    digitalWrite(AirIn, LOW);
    inflating = false;
    Serial.println("停止充氣");
  }
}

int getMode() {
  int s4 = digitalRead(pin4);
  int s5 = digitalRead(pin5);
  if (s4 == 1 && s5 == 1) return 0;
  if (s4 == 1 && s5 == 0) return 1;
  if (s4 == 0 && s5 == 1) return 2;
  if (s4 == 0 && s5 == 0) return 3;
  return -1;
}

void executeMode(int mode) {
  switch (mode) {
    case 0:
      {
        Serial.println("[模式0] 校正模式");
        calibrateInflationTimes();
        break;
      }

    case 1:
      {
        data1 = analogRead(FlexSensor1);
        Serial.print("[模式1] Flex Sensor: ");
        int flexValue = analogRead(FlexSensor1);
        Serial.print("FlexSensor 值: ");
        Serial.println(flexValue);

        // 讀取目前角度（使用 encoder counter）
        noInterrupts();
        int position = counter;
        interrupts();

        // 判斷是否需要上升，目前設定大於1000，可再另外調整
        if (flexValue > 1350 && position < 5) {
          Serial.println("Flex 過大，嘗試上升...");
          targetPos = position + 1;
          bangBangWithTimingControl(position, targetPos);
          counter = targetPos;  // 模擬控制完成
        }

        // 判斷是否需要下降（排氣），目前設定小於1000，可再另外調整
        else if (flexValue < 1000 && position > 0) {
          Serial.println("Flex 偏低，嘗試下降...");
          digitalWrite(AirIn, LOW);
          digitalWrite(AirOut, HIGH);
          delay(300);  // 洩氣時間，可依實際調整
          digitalWrite(AirOut, LOW);
          counter = position - 1;
        }
        break;
      }

    case 2:
      {
        Serial.println("[模式2] 控制編碼器+充氣控制");
        int pos;
        noInterrupts();
        pos = counter;
        interrupts();
        bangBangWithTimingControl(pos, targetPos);
        break;
      }

    case 3:
      {
        Serial.println("[模式3] 待機模式，保持排氣狀態");
        digitalWrite(AirIn, LOW);
        digitalWrite(AirOut, HIGH);
        break;
      }

    default:
      {
        Serial.println("[錯誤] 無效模式");
        break;
      }
  }
}
