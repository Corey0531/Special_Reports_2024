const int pin4 = 4; // 數位腳位 4 模式判別
const int pin5 = 5; // 數位腳位 5 模式判別

unsigned long lastUpdateTime = 0; // 儲存上次模式更新的時間
const unsigned long interval = 500; // 更新間隔，單位：毫秒

int currentMode = -1; // 當前模式 (-1 表示未初始化)

// 初始化
void setup() {
    pinMode(pin4, INPUT);
    pinMode(pin5, INPUT);
    
    
    Serial.begin(9600);
    
}

// 主迴圈
void loop() {
    // 讀取腳位狀態
    int state4 = digitalRead(pin4);
    int state5 = digitalRead(pin5);

    // 根據腳位狀態設定模式
    int newMode = getMode(state4, state5);

    // 若模式改變或超過間隔時間，執行模式功能
    unsigned long currentTime = millis();
    if (currentMode != newMode || currentTime - lastUpdateTime >= interval) {
        currentMode = newMode; // 更新模式
        lastUpdateTime = currentTime; // 更新時間

        // 執行對應模式功能
        executeMode(currentMode);
    }
}
// 判斷模式
int getMode(int state4, int state5) {
    if (state4 == 0 && state5 == 0) return 0; // 模式 0 紅點黑一
    if (state4 == 0 && state5 == 1) return 1; // 模式 1 紅空黑一
    if (state4 == 1 && state5 == 0) return 2; // 模式 2 紅點黑圈
    if (state4 == 1 && state5 == 1) return 3; // 模式 3 紅空黑圈
    return -1; // 不應到此
}

// 執行對應模式功能
void executeMode(int mode) {
    switch (mode) {
        case 0:
            Serial.println("模式 0：執行模式 0 的操作");
            break;
        case 1:
            Serial.println("模式 1：執行模式 1 的操作");
          
            
            break;
        case 2:
            Serial.println("模式 2：執行模式 2 的操作");
            break;
        case 3:
            Serial.println("模式 3：執行模式 3 的操作");
            break;
        default:
            Serial.println("未知模式");
            break;
    }
}