#ifndef BANGBANGCONTROL_H
#define BANGBANGCONTROL_H

#include <Arduino.h>

// 外部變數宣告
extern unsigned long inflateTimeTable[4];
extern bool isCalibrated;
extern const int AirIn;
extern const int AirOut;
extern volatile int counter;

// Bang-Bang 控制函數，使用查表進行控制
void bangBangWithTimingControl(int currentPos, int targetPos) {
  static unsigned long inflateStart = 0;
  static bool isInflating = false;

  if (!isInflating && currentPos < targetPos && targetPos <= 4) {
    // 根據差距計算總充氣時間
    unsigned long totalTime = 0;
    for (int i = currentPos; i < targetPos; i++) {
      totalTime += inflateTimeTable[i];
    }

    inflateStart = millis();
    isInflating = true;

    digitalWrite(AirIn, HIGH);
    digitalWrite(AirOut, LOW);
    Serial.println("啟動充氣");
  }

  // 停止充氣
  if (isInflating && (millis() - inflateStart >= 1)) {
    digitalWrite(AirIn, LOW);
    isInflating = false;
    Serial.println("停止充氣");
  }

  // 這裡可以加排氣控制
}

#endif

