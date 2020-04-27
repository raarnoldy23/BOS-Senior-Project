#include <Adafruit_ATParser.h>
#include <Adafruit_BLE.h>
#include <Adafruit_BLEBattery.h>
#include <Adafruit_BLEEddystone.h>
#include <Adafruit_BLEGatt.h>
#include <Adafruit_BLEMIDI.h>
#include <Adafruit_BluefruitLE_SPI.h>
#include <Adafruit_BluefruitLE_UART.h>
#include <RoboClaw.h>
#include <SoftwareSerial.h>
#include <Arduino.h>

SoftwareSerial serial(10,11);
RoboClaw roboclaw(&Serial, 10000);

#define address 0x80

//--------------------------Initial Setttings-----------------------------//
//-------------------------Serial Inputs----------------------------------//
int MainSw = 0; // Serial input switch
int Main_CheckLimits = 1;
int Manual_PositionMotor = 2;
int Position_OpenClose = 3;
int EstopSw = 4;
int DurCycle_Setup = 5;


//--------------------------Adjustable Variables--------------------------//
bool ClosedPosition = true;
bool OpenPosition = false;
bool StartStopDurabilitySw = false;
bool Estop = false;

//-----Main Program variables for Position Control-----//
bool OpenSwMain = false;
bool CloseSwMain = true;
bool DisableOpen = true;
bool DisableClose = false;
bool PositionControlSwitch = false;

int MotorSpeed = 69;
bool OpenOut = false; //need to set to break connection of output power pins
bool CloseOut = true;
bool ResetCount = false;
int PositionControlVal = 0; //set for positioning limits
bool OpenLimitSet = false;
bool CloseLimitSet = true;

bool Eenable = false;
int OpenLimit = 700;
int CloseLimit = 0;
bool SaveOpenSwitch = false;
bool SaveCloseSwitch = true;
bool DurabilityZero = true;
//Time StallUser - May use counter method
int RunTimeDura = 0; // May use counter method
int RunTimeSet = 7;
bool Stall = false;
bool StallDetectionAlert = false;
bool Cycle = false;  //Counter for cycles
int RunTimeElapsed = 0;  //recorded time during test
int Remaining = 0;
int MotorPosition = roboclaw.ReadEncM1(0x80);
//int SensedMotorSpeed = roboclaw.ReadSpeedM1(address, &status3, &valid3);

int TmrRun = 0; //timer for run time
bool CyclePause = false;
bool ClosePause = false;
int TmrCycle = 0;
int CycleLimit = 7; //can be set as user defined in the end from android
int CycleCount = 0; //counter for number of cycles completed
int StallTimer = 0; //Timer for stall time
//int motor_1_count = roboclaw.ReadEncM1(0x80);
//int MainSw = digitalRead(0(TX), INPUT);

//----------FUNCTIONS LIST----------//
/*void Setup()
void MainPrg()
void E_MainProgram_CheckLimits()
void E_MainProgram_breakmotor()
void E_ManControl_ButtonPressed()
void E_ManControl_PositionMotor()
void E_ManControl_Break()
void negative()
void RoofOpen()
void SaveLimits()
void Settings()
void E_PositionSwitch_SwitchPressed()
void E_PositionSwitch_OpenClose()
void E_PositionSwitch_Break()
void StallTime()
void durability()
void E_DurabilityCycle_StartTest()
void E_DurabilityCycle_Setup()
void E_DurabilityCycle_RunClose()
void E_DurabilityCycle_RunOpen()
void BreakMotor()
void RoofClose()
void EmStop()
//-----------------------------------//
*/

void setup(){
  Serial.begin(57600);
  roboclaw.begin(38400);
  //MotorPosition = roboclaw.SpeedAccelDeccelPositionM1(0x80,10000,2000,10000,15000,1);
  //MotorSpeed = 69;
  if ((MotorPosition +8) > 0){
    RoofOpen();
  }
  else if ((MotorPosition - 8) < 0){
    RoofClose();
  }
  else{
    BreakMotor();
  }
}

void loop(){
  //MotorSpeed = 32000;
  if (OpenLimit = 0){
    OpenSwMain = false;
  }

if (Serial.available() > 0){
    MainSw = Serial.read();

    if (MainSw == Main_CheckLimits)
    {
      ((OpenSwMain == true) || (CloseSwMain == true));
      E_MainProgram_CheckLimits();
    }
    
    if (MainSw == Manual_PositionMotor)
    {
      E_ManControl_PositionMotor();
    }

    if (MainSw == Position_OpenClose)
    {
      E_PositionSwitch_OpenClose();
    }

    if (MainSw == DurCycle_Setup)
    {
        E_DurabilityCycle_Setup();
    }
    else
    {
        EmStop();
    }
  }
}//end void loop
    
    /*    switch (MainSw){
        case 1: //MainTest
            if ((OpenSwMain == true) || (CloseSwMain = true)){
            E_MainProgram_CheckLimits();
            }
        break;

        case 2: //manual
            if (PositionControlSwitch = true){
                E_ManControl_PositionMotor();
            }
        break;

        case 3:   //SwitchLogic
            if ((OpenSwMain == true) || (CloseSwMain == true)){
                E_PositionSwitch_OpenClose();
            }
        break;

        case 4:
            if (Estop = false){
                E_DurabilityCycle_Setup();
            }
            else
            {
            EmStop();
            }
        break;
    }
}*/



void E_MainProgram_CheckLimits(){
    if (OpenSwMain == true){
      RoofOpen();
    }
    else if (CloseSwMain == true){
      RoofClose();
    }
    else{
      E_MainProgram_breakmotor();
    }

    if (MotorPosition >= (OpenLimit - 8)){
      E_MainProgram_breakmotor();
      OpenOut = false;
      OpenLimitSet = true;
      CloseLimitSet = false;
    }
    else if (MotorPosition <= (CloseLimit + 8)){
      E_MainProgram_breakmotor();
      CloseOut = false;
      OpenLimitSet = false;
      CloseLimitSet = true;
    }
}// end main check limits

void E_MainProgram_breakmotor(){
      OpenLimitSet = false;
      CloseLimitSet = false;
      BreakMotor();
}      // end main break motor

void E_ManControl_PositionMotor(){
    if ((MotorPosition <= OpenLimit) && (MotorPosition >= CloseLimit)){
      if ((MotorPosition - 8) > PositionControlVal){
        RoofClose();
      }
      else if ((MotorPosition + 8) < PositionControlVal){
        RoofOpen();
      }
      else{
        E_MainProgram_breakmotor();
      } 
    }
    else
    {
      E_MainProgram_breakmotor();
    }
}   // end Position motor

 void E_ManControl_Break(){
       BreakMotor();
}   // end control break


void negative(){
  if (MotorPosition < 0){
    MotorPosition = (MotorPosition + 3);
  }
}   //end negative

void RoofOpen(){
  roboclaw.SetEncM1(0x80,10000);
  roboclaw.ForwardM1(address, MotorSpeed); //second value is speed 0 - 127
  if (MotorPosition == 10000){
    OpenOut = true;
    CloseOut = false;
  }  
}   // end roof open

void SaveLimits(){
  if (SaveOpenSwitch == true){
    OpenLimit = roboclaw.ReadEncM1(0x80);
  }
  else{
      CloseLimit = 0;
  }

}   //end save limits

//void Settings(){
//   SaveLimits();
//   //MotorSpeed = 32000;


void E_PositionSwitch_OpenClose(){ //1
      if (OpenSwMain == true){
        RoofOpen();
      }
      else if (CloseSwMain == true){
        RoofClose();
      }
      else{
        E_PositionSwitch_Break();
      }
}   //end position open/close      
      
void E_PositionSwitch_Break(){
        OpenLimitSet = false;
        CloseLimitSet = false;
        BreakMotor();
}   // end positon sw break

void StallTime(){
  if ((StartStopDurabilitySw == true) && (TmrRun > StallTimer)){
    Stall = true;
    StallDetectionAlert = false;
  }
  else{
    Stall = false;
    StallDetectionAlert = true;
  }

  if (Stall == true){
    Estop = true;
    ClosePause = false;
    CyclePause = false;
  }
}   //end stall time


void durability(){

  //input configuration//
  if ((CycleLimit == 0) || (OpenLimit == 0)) {
    StartStopDurabilitySw = false;
  } 
  
  if (StartStopDurabilitySw == false) {
    Eenable = false;
  }

  if (StartStopDurabilitySw == false) {
    Estop = false;
  }

  if ((CycleLimit) && (CycleCount != 0)){
    StartStopDurabilitySw = false;
    TmrRun = false;
  }

}   //end durability

void E_DurabilityCycle_StartTest(){
       if ((StartStopDurabilitySw == true) && (Cycle != true)){     //cycle was previously cycle.q signifying leading edge
         E_DurabilityCycle_Setup();
       }
 }


void E_DurabilityCycle_Setup(){ //To home Position
      if (StartStopDurabilitySw = true){
        if ((MotorPosition + 8) > 0){
          RoofOpen();
        }
        else if ((MotorPosition +8) < 0){
            RoofClose();
        }
        else{
            BreakMotor();
            ClosePause = false;
            CyclePause = true;
            E_DurabilityCycle_RunClose();
        }          
      }
}   //end durability setup


void E_DurabilityCycle_RunClose(){ //run until upper limit is reached
      if (StartStopDurabilitySw == true){
          //Run.IN = false;
          if (MotorPosition <= OpenLimit){
            if (CyclePause == true){
              //Run.IN = true;
              RoofOpen();
            }
          }
          else{
            BreakMotor();
            //Cycle.CU = false;
            ClosePause = true;
            E_DurabilityCycle_RunOpen();
          }
      }
      else{
        E_DurabilityCycle_StartTest();
      }
}   //end durability run close

void E_DurabilityCycle_RunOpen(){ //Run until returned to Open
      if (StartStopDurabilitySw == true){
        if (MotorPosition >= CloseLimit){
          TmrRun = 0;
          RunTimeDura = RunTimeSet - RunTimeElapsed;
            if (ClosePause == true){
              //Run.IN = true;
              RoofClose();
            }
        }
        else{
          BreakMotor();
          //Cycle.CU = true;
          CyclePause = false;
          E_DurabilityCycle_StartTest();
        }
      }
    }   //end durablilty runopen


void BreakMotor(){
  OpenOut = false;
  CloseOut = false;
}

void RoofClose(){
  roboclaw.BackwardM2(address, MotorSpeed); // 64 is set to middle speed, ranges from 1-127
  if (MotorPosition == 0){
    OpenOut = false;
    CloseOut = false;
  }
}

void EmStop() {
  BreakMotor();
  if (Eenable == true){
    TmrRun = 0; //resets RunTime
    if ((MotorPosition + 8) < 0){
      RoofOpen();
    }
    else if ((MotorPosition - 8) > 0){
      RoofClose();
    }
    else{
      StartStopDurabilitySw = false;
      BreakMotor();
      Estop = false;
      CycleLimit = 0;
    }
  }
}   //end emergency stop
