// Ryan Arnoldy
// Bluetooth Motor control

#include "RoboClaw.h"

#define address 0x80

// create roboclaw object
RoboClaw roboclaw(&Serial1, 10000);


int ReadEncoderDelay = 250; // Encoder read speed 
int MotorSpeed  = 100 ; // speed of motor    



void setup() {
  Serial.begin(9600); //SM
  Serial2.begin(9600); //BLE
  roboclaw.begin(9600); //Start Roboclaw
  Serial.println("....");
}

void loop() {
  bool readEncoder = true; 
  String input = bluetooth();
  feedback();  
  selection(input);  
 


}


/*Function checks if bluetooth is available then returns value sent by user*/
String bluetooth() {
  // Listen for radio
  if (Serial2.available()) {
    //  Serial.print("receiving");
    //Serial.write(Serial1.read());
    String in = Serial2.readString(); // read value from user
    long  EncoderValue = roboclaw.ReadEncM1(address);
    return in; 
  }
}


void feedback() {

    long  EncoderValue = roboclaw.ReadEncM1(address);
    //Serial2.print("|");  
    Serial2.print(EncoderValue); // send encoder value to the tablet  
    Serial.print(EncoderValue); 
    Serial2.println("."); 
    Serial2.print("|");  
    delay(ReadEncoderDelay); 
  
  } 


void selection(String sel) {
  int cycleCount; // Store the desired value for cycles from the user. 
  if (sel.startsWith("cyc")) {
    Serial.print("Enter");
    String cyc_r = sel.substring(3,sel.length()); // slice string prefix
    cycleCount = cyc_r.toInt();   // convert string to integer value  
    Serial.print("The Cycle value is"); 
    Serial.println(cycleCount);
    }



  
}


/*Calls forward function*/
void Forward() {
  roboclaw.ForwardM1(address, MotorSpeed);

}


/*Calls Reverse function*/
void Reverse() {
  roboclaw.BackwardM1(address, MotorSpeed);

}


/*Calls Break */ 
void Break() {
  roboclaw.BackwardM1(address, 0);

}
