// 常數定義
const int dt = 100;  // PID 計算周期
const float kp = 1200, ki = 0.005, kd = 2;  // PID 參數
const float kp2 = 800, ki2 = 0.001, kd2 = 2;  // 另一組 PID 參數
const int encoderPinA = 12, encoderPinB = 13;  // 編碼器腳位
const int AirIn = 8;    //輸出調整五口三位閥
const int AirOut = 9;  //輸出調整五口三位閥
const int pin4 = 4; // 數位腳位 4 模式判別
const int pin5 = 5; // 數位腳位 5 模式判別
volatile int encoderPos = 0;  // 編碼器位置

// 變數定義
float previous_error = 0, error = 0, P, I, D, PID, P2, I2, D2, PID2;
unsigned long pre = 0, time = 0, pre2 = 0;
int Degree = 0, Mode = 0, ArmSpeed = 0, state = 0;
int data1 = 0;  // 定義 data1 變數
char getstr1;  // 定義 getstr1 變數

// 設定編碼器中斷
void encoderISR() {
  if (digitalRead(encoderPinA) == HIGH) {
    encoderPos += (digitalRead(encoderPinB) == LOW) ? 1 : -1;
  } else {
    encoderPos += (digitalRead(encoderPinB) == HIGH) ? 1 : -1;
  }
}

void setup() {
  Serial.begin(15200);
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  pinMode(AirIn, OUTPUT);   
  pinMode(AirOut, OUTPUT);  
  pinMode(4, INPUT);  // 設定腳位 4 為輸入
  pinMode(5, INPUT);  // 設定腳位 5 為輸入
  attachInterrupt(digitalPinToInterrupt(encoderPinA), encoderISR, CHANGE);
}

void loop() {
  if (Serial.available()) {
    getstr1 = Serial.read();  // 從串口讀取單個字元並存入 getstr1
  }

  if (getstr1 == 'a') Mode = 1;
  else if (getstr1 == 'b') Mode = 2;
  else if (getstr1 == 'c') Mode = 3;
  else if (getstr1 == 'n') Mode = 4;

  switch (Mode) {
    case 1: FingerMode(); break;
    case 2: PhoneMode(); break;
    case 3: AutoMode(); break;
    case 4: Stop(); break;
    default: break;
  }

  // 輸出 debug 資訊
  if (millis() - pre2 > 100) {
    pre2 = millis();
    Serial.print("EncoderPos: "); Serial.println(encoderPos);
    Serial.print("Error: "); Serial.println(error);
    Serial.print("PID: "); Serial.println(PID);
  }
}

// PID 計算
float calculatePID(int target, int current) {
  error = target - current;
  P = kp * error;
  I += ki * error;
  D = kd * (error - previous_error) / dt;
  PID = P + I + D;
  previous_error = error;
  return PID;
}

// 控制輸出
void controlOutput(float pid) {
  if (abs(error) > 5) {
    digitalWrite(AirIn, pid > 0);
    digitalWrite(AirOut, pid < 0);
    Serial.println(pid > 0 ? "up" : "down");
  } else {
    digitalWrite(AirIn, 0);
    digitalWrite(AirOut, 0);
    Serial.println("stop");
  }
}

// 模式功能
void FingerMode() {
  if (millis() > time + dt) {
    time = millis();
    int pid = calculatePID(data1, encoderPos);
    controlOutput(pid);
  }
}

void PhoneMode() {
  if (millis() > time + dt) {
    time = millis();
    int pid = calculatePID(Degree, encoderPos);
    controlOutput(pid);
  }
}

void AutoMode() {
  if (millis() > time + dt) {
    time = millis();
    float y = ((Degree - 620) / 2) * sin(millis() * (3.14 / 4) / 1000) + ((Degree + 620) / 2);
    int pid = calculatePID(y, encoderPos);
    controlOutput(pid);
  }
}

void Stop() {
  digitalWrite(AirIn, LOW);  
  digitalWrite(AirOut, LOW);
  Mode = 0; // 停止模式
}
