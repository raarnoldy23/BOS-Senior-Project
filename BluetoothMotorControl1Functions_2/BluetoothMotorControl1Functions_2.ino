#include <RoboClaw.h>

// Ryan Arnoldy
// Bluetooth Motor control

#include "RoboClaw.h"

#define address 0x80 

// create roboclaw object
RoboClaw roboclaw(&Serial1, 10000);

/*Settings*/
int ReadEncoderDelay = 250; // Encoder read speed
int MotorSpeed  = 100 ; // speed of motor
/*Hall Vars */
int hall1; // hall count
int hall2;
long EncoderValue; // current encoder value
int openlimit;
int closelimit = 0; 

/*Event timing*/ 
/*Event timeing intervals*/
const unsigned long Read_BluetoothInput = 1200;  // 1
unsigned long last1 = 0; 
const unsigned long Read_Encoder = 0; // 2 NOT SURE OF TIMING INTERVAL YET!!!
unsigned long last2 = 0;
const unsigned long User_Selection = 1200; // 3     
unsigned long last3 = 0;  



/*Testing Parameters*/
int closedelay; // delay before starting next
int opendelay;
int cyclecount; // cycles to be run
int currentcycle; // cycle counter
/*LEDS*/
#define PowerLED  13 // shared with programming port on the board 
#define StatusLED 12
#define ConnectedLED 11
#define ErrorLED     10




void setup() {
  Serial.begin(9600); //SM
  Serial2.begin(9600); //BLE
  roboclaw.begin(9600); //Start Roboclaward
  pinMode(PowerLED, OUTPUT);
  pinMode(StatusLED, OUTPUT);
  pinMode(ConnectedLED, OUTPUT);
  pinMode(ErrorLED, OUTPUT);
}

void loop() {
  unsigned long currentTime = millis(); // run time clock   
  digitalWrite(PowerLED, HIGH);
  String Input; // holds user input 
  /*Event Timing*/  
  if(currentTime - last1 >= Read_BluetoothInput){ 
    input = bluetooth();
    last1 = currentTime; 
  }
  
  bool readEncoder = true;
  input = bluetooth();
  feedback();
  selection(input);
}


/*Function checks if bluetooth is available then returns value sent by user*/
String bluetooth(long ) {
  // Listen for radio
  if (Serial2.available()) {

    String in = Serial2.readString(); // read value from user
    long  EncoderValue = roboclaw.ReadEncM1(address);
    //Serial.print("Reading:  ");
    Serial.println(in); // print value read in
    return in;
  }
}

/*Read feedback from the motors*/
void feedback() {

  EncoderValue = roboclaw.ReadEncM1(address);
  Serial2.print(EncoderValue); // send encoder value to the tablet
  Serial.print(EncoderValue);
  Serial2.println(".");
  Serial2.print("|");
  delay(ReadEncoderDelay);

}

/*Manages input recieved front the user*/
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
    closelimit = EncoderValue ;
  }
  else if (sel.startsWith("rsl")) {
    openlimit = 0;
    closelimit = 0;

  }
  else if (sel.startsWith("rsh")) {
    EncoderValue = 0;
  }
  
  /*Set Open Limit */
  else if (sel.startsWith("opl")) {
    openlimit = EncoderValue ;

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

void ResumeTest() {
  // Return home
  if (EncoderValue != 0) {
    ReturnHome();
  }
  // Test Cycle 
  while(currentcycle <= cyclecount) {
    // OPEN
    if (EncoderValue< openlimit){
      Forward();
    }
    // CLOSE 
    else if (EncoderValue > openlimit){
      Reverse(); 
    }
    currentcycle ++; 
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

  if (EncoderValue >= 0) {
    while (EncoderValue >= 0) {
      Forward(); // May need to change to reverse
    }
  }
  else if (EncoderValue <= 0) {
    while (EncoderValue <= 0) {
      Reverse();
    }
  }
}


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
  
