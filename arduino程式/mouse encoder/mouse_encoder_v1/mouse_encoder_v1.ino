const int encoderPinA = 2;
const int encoderPinB = 3;

volatile int encoderCount = 0;

void setup() {
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), handleEncoderInterrupt, CHANGE);
  Serial.begin(9600);
}

void loop() {
  Serial.println(encoderCount);
  delay(100);
}

void handleEncoderInterrupt() {
  if (digitalRead(encoderPinA) == digitalRead(encoderPinB)) {
    encoderCount++;
  } else {
    encoderCount--;
  }
}
