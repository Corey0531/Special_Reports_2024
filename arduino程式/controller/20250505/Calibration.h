#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>

// 外部變數宣告
extern volatile int counter;
extern unsigned long inflateTimeTable[5];
extern const int AirIn;
extern const int AirOut;
extern bool isCalibrated;

void calibrateInflationTimes() {
  Serial.println("開始自動校準...");
  counter = 0;

  for (int i = 0; i < 5; i++) {
    unsigned long startTime = millis();

    while (counter <= i) {
      if (millis() - timeout > 10000) {  // 超過 10 秒則超時
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
    Serial.print("→");
    Serial.print(i + 1);
    Serial.print(" = ");
    Serial.print(inflateTimeTable[i]);
    Serial.println(" ms");

    delay(500);
  }

  isCalibrated = true;
  Serial.println("校準完成！");
}

#endif