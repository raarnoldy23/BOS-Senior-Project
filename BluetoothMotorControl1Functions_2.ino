// Ryan Arnoldy
// Bluetooth Motor control

#include "RoboClaw.h"

#define address 0x80

// create roboclaw object
RoboClaw roboclaw(&Serial1, 10000);

void setup() {
  Serial.begin(9600); //SM
  Serial2.begin(9600); //BLE
  roboclaw.begin(9600); //Start Roboclaw
  Serial.println("....");
}

void loop() {
  bluetooth();




}

void bluetooth() {
  // Listen for radio
  if (Serial2.available()) {
    //  Serial.print("receiving");
    //Serial.write(Serial1.read());
    char in = Serial2.read(); // read value from user
    long  EncoderValue = roboclaw.ReadEncM1(address);
    Serial2.print("|");
    Serial.print(EncoderValue); // send encoder value to the tablet
    Serial2.print("|");
    Serial.print(in);

    selection(in); // send read value to selection function
  }
}


void selection(char sel) {

  if (sel == '1') {
    Forward();
    Serial.print("Forward");
  }
  else if (sel == '2') {
    Reverse();
    Serial.print("reverse");
  }
  else if (sel = '0') {
    Break();
  }
  else {
    Serial.println("Enter a valid input");
  }
}


void Forward() {
  roboclaw.ForwardM1(address, 100);
  Serial.print("Forward");
  delay(500);
}


void Reverse() {
  roboclaw.BackwardM1(address, 100);
  Serial.print("Reverse");
  delay(500);
}


void Break() {
  roboclaw.BackwardM1(address, 0);
  Serial.print("Break");
}
