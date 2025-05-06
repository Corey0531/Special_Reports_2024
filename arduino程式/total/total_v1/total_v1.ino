#include <SoftwareSerial.h>
// for Arduino nano 
// 定義軟體序列埠腳位
SoftwareSerial bluetooth(1, 2); // RX, TX
// 定義數位腳位
const int pin4 = 4; // 數位腳位 4 模式判別
const int pin5 = 5; // 數位腳位 5 模式判別
const int encoderA = 6; // 數位腳位 6 encoder
const int encoderB = 7; // 數位腳位 7 encoder
const int AirIn = 8;    //輸出調整五口三位閥
const int AirOut = 9;  //輸出調整五口三位閥
int FlexSensor1 = A0; //裝在手指上的感測器

int xval = 0;    //Ｘ軸變數
int yval = 0;    //Ｙ軸變數
int zval = 0;    //Z軸變數(SW)
int counter = 0;// 編碼器計數
int aState, aLastState = 0;//編碼器狀態
int data1 = 0; 

unsigned long lastUpdateTime = 0; // 儲存上次模式更新的時間
const unsigned long interval = 500; // 更新間隔，單位：毫秒

int currentMode = -1; // 當前模式 (-1 表示未初始化)

// 初始化
void setup() {
    pinMode(pin4, INPUT_PULLUP);
    pinMode(pin5, INPUT_PULLUP);
    pinMode(encoderA, INPUT);
    pinMode(encoderB, INPUT);
    pinMode(FlexSensor1, INPUT);

    Serial.begin(9600);
    bluetooth.begin(9600); // 與藍牙模組通訊
    Serial.println("初始化完成，等待藍牙模組...");
    aLastState = digitalRead(encoderA);
}

// 主迴圈
void loop() {
    // 讀取腳位狀態
    int state4 = digitalRead(pin4);
    int state5 = digitalRead(pin5);

    // 根據腳位狀態設定模式
    int newMode = getMode(state4, state5);
    data1 = analogRead(FlexSensor1);

    // 若模式改變或超過間隔時間，執行模式功能
    unsigned long currentTime = millis();
    if (currentMode != newMode || currentTime - lastUpdateTime >= interval) {
        currentMode = newMode; // 更新模式
        lastUpdateTime = currentTime; // 更新時間

        // 執行對應模式功能
        executeMode(currentMode);
    }
}
// 讀取編碼器的函式
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

// 判斷模式
int getMode(int state4, int state5) {
    if (state4 == 1 && state5 == 1) return 0; // 模式 0 紅點黑一
    if (state4 == 1 && state5 == 0) return 1; // 模式 1 紅空黑一
    if (state4 == 0 && state5 == 1) return 2; // 模式 2 紅點黑圈
    if (state4 == 0 && state5 == 0) return 3; // 模式 3 紅空黑圈
    return -1; // 不應到此
}

// 執行對應模式功能
void executeMode(int mode) {
    switch (mode) {
        case 0:
            Serial.println("模式 0：執行模式 0 的操作");
            Serial.println(data1);

            break;
        case 1:
            Serial.println("模式 1：執行模式 1 的操作");
            
            
            break;
        case 2:
            Serial.println("模式 2：執行模式 2 的操作");
            digitalWrite(AirIn,1);
            digitalWrite(AirOut,1); 
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


