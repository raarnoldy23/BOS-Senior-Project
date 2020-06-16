#include <RoboClaw.h>
// Ryan Arnoldy
// Bluetooth Motor control
/**/ 

#include "RoboClaw.h"
#define address 0x80 
// create roboclaw object
RoboClaw roboclaw(&Serial1, 10000);
/*Settings*/
int ReadEncoderDelay = 250; // Encoder read speed
int MotorSpeed  = 100 ; // speed of motor
/*Hall Vars */
int hall1; // hall count returned  by encoders 
int hall2;
long EncoderValue1 ; // current encoder value
long EncoderValue2; 
int openlimit; 
int closelimit = 0; 
/*Event timeing intervals*/
unsigned long currentTime = millis(); // run time clock   
const unsigned long Read_BluetoothInput = 1200;  // 1
unsigned long lastBluetoothInput = 0; 
const unsigned long Read_Encoder = 500; // 2 NOT SURE OF TIMING INTERVAL YET!!!
unsigned long lastEncoder = 0;
const unsigned long User_Selection = 1200; // 3     
unsigned long lastSelection = 0;  
/*Testing Parameters*/
int closedelay; // delay before starting next
int opendelay;
int cyclecount; // # of cycles to be run 
int currentcycle; // cycle counter
/*LEDS*/
#define StatusLED 13 // shared with programming port on the board 
#define ConnectedLED 11 // Bluetooth Status 
#define ErrorLED     10 
/*LED Timing*/
int lastFlashStatus = 0;
const unsigned long StatusFlash = 250; 


void setup() {
  Serial.begin(9600); //SM
  Serial2.begin(9600); //BLE
  roboclaw.begin(9600); //Start Roboclaward
  pinMode(StatusLED, OUTPUT);
  pinMode(ConnectedLED, OUTPUT);
  pinMode(ErrorLED, OUTPUT);
}


void  loop() {
  String input; // holds user input 
  /*Event Timing*/  
  if(currentTime - lastBluetoothInput >= Read_BluetoothInput){ 
    Serial.println("Event1");
    input = bluetooth();
    lastBluetoothInput = currentTime; 
  }
  if(currentTime - lastSelection >= User_Selection ){
    Serial.println("Event2"); 
    selection(input); 
    lastSelection = currentTime; 
  } 
  if(currentTime- lastEncoder >= Read_Encoder){
    Serial.println("Event3"); 
    feedback(); 
    lastSelection = currentTime;  
  }

  
  
}


/*Function checks if bluetooth is available then returns value sent by user*/
String bluetooth() {
  // Listen for radio
  if (Serial2.available()) {
    String in = Serial2.readString(); // read value from user
    long  EncoderValue1  = roboclaw.ReadEncM1(address);
    //Serial.println(in); // print value read in
    return in;
  }
}

/*Return Feedback to UI*/
void feedback() {
  EncoderValue1  = roboclaw.ReadEncM1(address);
  EncoderValue2 = roboclaw.ReadEncM2(address);  

  Serial2.print(EncoderValue1 ); // send encoder value to the tablet
  Serial.print(EncoderValue1 ); 
  Serial2.println(".");
  Serial2.print("|");
  delay(ReadEncoderDelay); // Remove encoder delay 

  Serial2.print(EncoderValue2);  
  Serial2.println("."); 
  Serial2.print("|");   
  delay(ReadEncoderDelay);  

  Serial2.print(currentcycle);
  Serial2.println("."); 
  Serial2.print("|");  
  delay(ReadEncoderDelay);  

  Serial2.print(openlimit); 
  Serial2.println("."); 
  Serial2.print("|");  
  delay(ReadEncoderDelay);  

  Serial2.print(closelimit); 
  Serial2.println("."); 
  Serial2.print("|");  
  delay(ReadEncoderDelay);  
}


/*Manages input 121 q33 front the user*/
void selection(String sel) {
  if (sel.startsWith("cyc")) {
    String cyc_r = sel.substring(3, sel.length()); // slice string prefix
    cyclecount = cyc_r.toInt();   // convert string to integer value
  }
  /*Delay between Open and close cycle*/
  else if (sel.startsWith("odl")) {
    String opendelay_r = sel.substring(3, sel.length());
    opendelay = opendelay_r.toInt();
    opendelay = opendelay * 1000;
  }
  else if (sel.startsWith("cdl")) {
    String closedelay_r = sel.substring(3, sel.length());
    closedelay = closedelay_r.toInt();
    closedelay = closedelay * 1000;
  }
  /*Set close Limit*/
  else if (sel.startsWith("cll")) {
    closelimit = EncoderValue1  ; 
  }
  // reset limis
  else if (sel.startsWith("rsl")) {
    //  open limit should be set to 0 
    openlimit = 0;
    closelimit = 0;

  }
  else if (sel.startsWith("rsh")) {
    EncoderValue1  = 0;
  }
  /*Set Open Limit */
  else if (sel.startsWith("opl")) {
    openlimit = EncoderValue1  ;
  }
  else if (sel.startsWith("fwd")) {
    Forward();
  }
  else if (sel.startsWith("rev")) {
    Reverse();
  }
  else if (sel.startsWith("brk")) {
    Break();
  }
  else if (sel.startsWith("sta")) {
    ResumeTest();
  }
  else if (sel.startsWith("rst")) {
    ResetTest();
  }
  else if (sel.startsWith("pau")) {
    PauseTest();
  }
}


// Run Test
void ResumeTest() {
  // Return home
  if (EncoderValue1  != 0) {
    ReturnHome();
  }
  // Test Cycle 
  while(currentcycle <= cyclecount) {
    TestRunningLED();  
    // OPEN
    if (EncoderValue1 < openlimit){
      Forward();
    }
    // CLOSE 
    else if (EncoderValue1  > openlimit){
      Reverse(); 
    }
    currentcycle ++; 
    if (currentcycle ==){} 
  }

}


void ResetTest() {
  Break();
  ReturnHome(); 
  currentcycle = 0; 
  cyclecount = 0; 
}


void PauseTest() {
  Break(); 
  ReturnHome(); 
}


/* Return to home position */
/* May need to change variables based on direction +- */
/* Function may also need to add hardcode off set var to compensate for delays*/
void ReturnHome() {

  if (EncoderValue1  >= 0) {
    while (EncoderValue1  >= 0) {
      Forward(); // May need to change to reverse
    }
  }
  else if (EncoderValue1  <= 0) {
    while (EncoderValue1  <= 0) {
      Reverse();
    }
  }
}


/*Motor Functions*/ 
//Motor1
void Forward() {
  roboclaw.ForwardM1(address, MotorSpeed);
}


void Reverse() {
  roboclaw.BackwardM1(address, MotorSpeed);
}


void Break() {
  roboclaw.BackwardM1(address, 0);
  roboclaw.BackwardM2(address, 0);
}
//Mortor2


/*LED Functions*/
// Indicates test is running Status LED Blinks 
void TestRunningLED(){
  if(currentTime - lastFlashStatus >= StatusFlash){
    int StatusLEDState = LOW; 
    if (StatusLED == LOW){
      StatusLEDState = HIGH; 
    }
    else{
      StatusLEDState = LOW; 
    }
    digitalRead(StatusLED, StatusLEDState); 
  }
}

  
 









































  
