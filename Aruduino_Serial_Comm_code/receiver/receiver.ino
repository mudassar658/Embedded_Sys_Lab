#include <SoftwareSerial.h>

SoftwareSerial SwSerial(10, 11); // RX, TX

void setup() {
  Serial.begin(9600);
  SwSerial.begin(9600);
}

void loop() {
  if (SwSerial.available()) {
    int inByte = SwSerial.read();
    Serial.write(inByte);
  }
}
