
int AirOut = 9; 

void setup() {

  Serial.begin(9600);
  pinMode(AirOut, OUTPUT);

}
void loop() {
 
  digitalWrite(AirOut,LOW);
  delay(5000);
  digitalWrite(AirOut,HIGH);
  delay(5000);
 

}