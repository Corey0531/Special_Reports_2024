int xPin = 0;  //X軸 接類比A0
int yPin = 1;  //Y軸 接類比A1
int zPin = 2;  //Z軸 接類比A2
  
int xval=0;    //Ｘ軸變數
int yval=0;    //Ｙ軸變數
int zval=0;    //Ｚ軸變數


void setup() {
  Serial.begin(9600);  // 視窗baud rate
}

void loop() {

  xval = analogRead(xPin);   //讀取X軸數值到xval變數
  yval = analogRead(yPin);   //讀取Y軸數值到yval變數
  zval = analogRead(zPin);   //讀取Z軸數值到zval變數

  char buf[100];
  sprintf(buf, "VRx=%d, VRy=%d, SW=%d", xval, yval, zval);
  Serial.println(buf);

  delay(100);                    
 
}
