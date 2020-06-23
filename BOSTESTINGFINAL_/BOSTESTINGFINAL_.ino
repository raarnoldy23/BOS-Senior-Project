// Run motor with table control
#include "RoboClaw.h"
#define address 0x80
RoboClaw roboclaw(&Serial1, 10000);
/*Testing Variable*/
int motorSpeed = 127; // Speed of the motor
int setupSpeed = 64;
int breakMotor = 0;
long encoder1Value; // motor 1 encoder count
long openLimit1 = 0;
long closeLimit1 = 0; // Home position
int cyclecount = 0; // number of cycles to run
int count = 0; // cycle counter
bool testComplete = false; // TESTING exit variable
/*Timing Variables*/
unsigned long Time = millis(); // Run time clock
unsigned long encoderCountInterval = 250;
unsigned long lastEncoderCount = 0;
unsigned long StatusFlash = 250;
unsigned long lastFlashStatus = 0;
/*Status LEDS*/
#define ConnectionLED 11
#define ErrorLED 12
#define StatusLED 13


void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  roboclaw.begin(9600);
}


void loop() {
  String in;
  /*Function Calls*/
  in = readinput();
  selection(in);
  feedback();  
}


/*Function that reads user input*/
String readinput() {
  String in; //Store user input
  // Listen for radio
  /*Change back to Serial2*/
  if (Serial2.available()) {
    in = Serial2.readString(); // read valhall effect sensor the same as a quadratureue from user
    //  long  EncoderValue1  = roboclaw.ReadEncM1(address);
    Serial.print("Input:  ");
    Serial.println(in); // print value read in
    return in;
  }
}

void feedback(){
  /*Return Feedback */
  encoder1Value = roboclaw.ReadEncM1(address);
  Serial2.print(encoder1Value); // Li 1
  Serial2.print("|");
  Serial2.print(openLimit1); // Li 2
  Serial2.print("|");
  Serial2.print(closeLimit1); // Li 3
  Serial2.print("|");
  Serial2.print(cyclecount); // Li 4
  Serial2.print("|");
  Serial2.println(count); // Li 5
  delay(250); // remove
  }


/*Calls Subfunction based on user input*/
void selection(String in) {
  /*MotorControl*/
  // forward
  if (in.startsWith("fwd")) {
    forward();
  }
  // reverse
  else if (in.startsWith("rev")) {
    reverse();
  }
  // break motor 
  else if (in.startsWith("brk")) {
    breakmotor();
  }
  /*Testing*/
  // get the number of cycles from user
  else if (in.startsWith("cyc")) {
    String cyc_r = in.substring(3, in.length());
    cyclecount = cyc_r.toInt();
  }
  else if (in.startsWith("sta")) {
    testComplete = false;  
    durabilityTest();
    count = 0;
  }
  // reset test 
  else if (in.startsWith("rst")) {
    count = 0; 
    cyclecount = 0; 
    // add open/close delay     
  }
  // set close limit 
  else if (in.startsWith("cll")) {
    closeLimit1 = encoder1Value;
  }
  // set open limit
  else if (in.startsWith("opl")) {
    openLimit1 = encoder1Value;
  }
  // reset limits
  else if (in.startsWith("rsl")) {
    closeLimit1 = 0;
    openLimit1 = 0;
  }
  // reset hall 
  else if (in.startsWith("rsh")) {
    encoder1Value = 0;
    //DEBUG 
  }
}


/*Testing functions*/
//long readEncoder() {
//  encoder1Value = roboclaw.ReadEncM1(address);
//  Serial.println(encoder1Value);
//}


void durabilityTest() {
  int controlVar = 1;
  // testComplete = false;
  while (count != cyclecount && testComplete != true) {
    switch (controlVar) {
      // home function
      case 1:
        while (encoder1Value != closeLimit1) {
          encoder1Value = roboclaw.ReadEncM1(address);
          //Serial.print(encoder1Value  );
          if (encoder1Value == closeLimit1) {
            roboclaw.ForwardM1(address, breakMotor);
            //          Serial.println("3");
          }
          else if (encoder1Value > closeLimit1 ) {
            roboclaw.BackwardM1(address, setupSpeed);
            //          Serial.println("1");
          }
          else if (encoder1Value < closeLimit1) {
            roboclaw.ForwardM1(address, setupSpeed);
            //          Serial.println("2");
          }
        }
        controlVar = 2;
        break;

      // Run Open
      case 2:
        while (encoder1Value <= openLimit1) {
          encoder1Value = roboclaw.ReadEncM1(address);
          roboclaw.ForwardM1(address, motorSpeed);
        }
        controlVar = 3;
        break;

      // Run Close
      case 3:
        while (encoder1Value >= closeLimit1) {
          encoder1Value = roboclaw.ReadEncM1(address);
          roboclaw.BackwardM1(address, motorSpeed);
        }
        controlVar = 4;
        break;

      // checkCycle
      case 4:
        count++;
        feedback();  
//        Serial.print("Count:  ");
//        Serial.println(count);
//        Serial.print("Cycle Count:");
//        Serial.println(cyclecount);

        if (count == cyclecount) {
         controlVar = 5;
//        Serial.print("controlVar:  ");
//        Serial.println(controlVar);
//        Serial.println("END");
        roboclaw.BackwardM1(address, 0);
        testComplete = true;
        break;
        }
        else {
          controlVar = 2;
        }
//        Serial.print("controlVar:  ");
//        Serial.println(controlVar);
        break;

      // End test
      case 5:
//        Serial.println("END");
        roboclaw.BackwardM1(address, 0);
        testComplete = true;
        break;
    }
  }
}




/*Motor control functions*/
void forward() {
  Serial.println("Forward");
  roboclaw.ForwardM1(address, motorSpeed);
}


void reverse() {
  Serial.println("Reverse");
  roboclaw.BackwardM1(address, motorSpeed);
  //    EncoderValue1  = roboclaw.ReadEncM1(address);
  //    Serial.print(EncoderValue1);
}


void breakmotor() {
  Serial.println("Break");
  roboclaw.BackwardM1(address, breakMotor);
}


/*LED Functions*/
// Indicates test is running Status LED Blinks
void TestRunningLED() {
  if (Time - lastFlashStatus >= StatusFlash) {
    int StatusLEDState = LOW;
    if (StatusLED == LOW) {
      StatusLEDState = HIGH;
    }
    else {
      StatusLEDState = LOW;
    }
    digitalWrite(StatusLED, StatusLEDState);
  }
  lastFlashStatus = Time;
}
// Test Complete 
void TestCompleteLED(){
  
  
  
  }

//void TestError(){
//  if(currentTime - lastFlashError >= StatusFlash){
//    int ErrorLEDState = LOW;
//    if (ErrorLEDState == LOW){
//       ErrorLEDState = HIGH;
//    }
//    else{
//      ErrorLEDState= LOW;
//    }
//    digitalWrite(ErrorLED, ErrorLEDState);
//  }
//}
