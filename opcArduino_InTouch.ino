#include <SoftwareSerial.h>
SoftwareSerial Serial2(2, 3);  // 2 rx, 3 tx
#include <OPC.h>
#include <Bridge.h>
#include <Ethernet.h>
#include <SPI.h>

OPCSerial aOPCSerial;
String data;

int analogPin = 3;
int sensor;
int val;
bool cw, ccw;
int speed, dutyCycle;
float voltIn, voltMo;

int enB = 10;
int in3 = 8;
int in4 = 9;

bool statCW(const char *itemID, const opcOperation opcOP, const bool value) {
  return cw;
}
bool statCCW(const char *itemID, const opcOperation opcOP, const bool value) {
  return ccw;
}
int statSpeed(const char *itemID, const opcOperation opcOP, const int value) {
  return speed;
}
int statvoltMo(const char *itemID, const opcOperation opcOP, const int value) {
  return voltMo;
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  aOPCSerial.setup();

  aOPCSerial.addItem("cw", opc_read, opc_bool, statCW);
  aOPCSerial.addItem("ccw", opc_read, opc_bool, statCCW);
  aOPCSerial.addItem("speed", opc_read, opc_int, statSpeed);
  aOPCSerial.addItem("voltMo", opc_read, opc_int, statvoltMo);
}

void loop() {
  aOPCSerial.processOPCCommands();
  val = analogRead(A0);
  voltIn = map(val, 0, 1023, 0, 500);
  if (voltIn >= 0 && voltIn <= 100) {
    cw = 0;
    ccw = 0;
    speed = 0;
    voltMo = 0;
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    analogWrite(enB, 0);
  } else if (voltIn > 100 && voltIn <= 300) {
    if (ccw == 1) {
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      analogWrite(enB, 0);
      delay(2000);
    }
    cw = 1;
    ccw = 0;
    speed = map(voltIn, 101, 300, 0, 1800);
    dutyCycle = map(speed, 0, 1800, 0, 255);
    voltMo = map(dutyCycle, 0, 255, 0, 900);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enB, dutyCycle);
  } else if (voltIn > 300 && voltIn <= 305) {
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    analogWrite(enB, 0);
    speed = 0;
    voltMo = 0;
  } else if (voltIn > 305 && voltIn <= 500) {
    if (cw == 1) {
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      analogWrite(enB, 0);
      delay(2000);
    }
    cw = 0;
    ccw = 1;
    speed = map(voltIn, 306, 500, 0, 1800);
    dutyCycle = map(speed, 0, 1800, 0, 255);
    voltMo = map(dutyCycle, 0, 255, 0, 900);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    analogWrite(enB, dutyCycle);
  }
  Serial2.println(val);
}
