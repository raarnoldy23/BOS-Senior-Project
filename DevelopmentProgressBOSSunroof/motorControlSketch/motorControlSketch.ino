// +++++++++++++++++++++++++++++++++++++ Settings(adjustable) +++++++++++++++++++++++++++++++++++++++ //  

// ------------------------------ VAR adjustable ----------------------------- //  

int posClosed = 130;  

int posOpen = 770;

int driveTrigger = 0;

int posSoftStopClose = (posClosed + 50); 

int posSoftStopOpen = (posOpen - 50);  

int posfrontZV = posClosed + 25; 

int SpeedNormalPWM = 150;  

int SpeedSoftStopPWM = 120; float pinchoffRefField = 0.70;

int LockingCounterEKS = 0;  

// ----------------------------------------------------------------------------- //  

// ++++++++++++++++++ Settings(fixed)++++++++++++++++++++++++++++++++++++++ //  

// ---------------------------- VAR systemposition  ----------------------------- //  

int posSystemMitte = (posOpen + posClosed) / 2;  

// -----------------------------------------------------------------------------//  

// ---------------------------- VAR state machine  ---------------------------- //  

int state = 0; // Current state of the system  

unsigned long lastRunTimeCondition = 0;  

unsigned long cycleTimeState = 1500; 

// ----------------------------------------------------------------------------- //  

// --------------------------- VAR turn-angle sensor logic --------------------- //  

unsigned long lastRunTime = 0;  

unsigned long cycleTime = 1000;  

int rotaryanglevalue = 0;  // values ​​of the turn angle sensor max: 900 min: 150  

int rotaryanglechangevalue = 0; 

// bool rotaryanglechangevalue = false; // has the rotation angle changed  

// -----------------------------------------------------------------------------//  

// ---------------------------- VAR ZV-Steller  ---------------------------------//  

bool lockFailig = false; // locking procedure finished  

bool unlocking = false; // Unlock procedure ends  

int StateZV = 0; // Release key on ZV module  

int relaydrivepin = 7;

int relayControlPin = 7; // the relay controls the ZV  

int ValueZVPin = A4; // pin ZV  

int ValueZV = 0; // values ​​of ZV bool ZV800 = false;  

bool ZV10 = false; 

unsigned long lastRunTimeZV = 0;  

// unsigned long cycleTimeZV = 200;  

unsigned long lastRunTimeRelay = 0;  

unsigned long cycleTimeRelay = 200; 


// -----------------------------------------------------------------------------//  

//---------------------------- VAR user control ----------------------------//  

int drivepin = 12;  

bool controlTrigger = false; // keypress by the user
  

//-----------------------------------------------------------------------------//  

// ---------------------------- VAR motor parameters -----------------------------//  

int pwmPin;  

int inA; 

int inB; 

int rotationDirection;  

int driveVoltagePercentage; 


 //-----------------------------------------------------------------------------//  

//---------------------------- VAR current sensor --------------------------------//

int currentsensorpin = A6;  

double currentsensorvalue = 0; 


// ----------------------------------------------------------------------------//  

//---------------------------- VAR Initialization  ----------------------------//  

int initCondition = 0;  

bool initFinished = LOW;  



//------------------------------ Program Begins ---------------------------------//
// ----------------------------------------------------------------------------- //  
void setup() 
{ 

    Serial.begin (9600);  

    pinMode (driveTrigger, INPUT);  

    pinMode (rotaryanglevalue, INPUT);  

    pinMode (currentsensorpin, INPUT);  

    pinMode (ValueZVPin, INPUT);  

    pinMode (relaydrivepin, OUTPUT);  

    pinMode (ValueZVPin, INPUT);  

    setMotorPin (9, 5, 4);  

    rotaryanglevalue = analogRead (A0);  

    rotaryanglechangevalue = rotaryanglevalue;  

    driveTrigger = digitalRead (drivepin); 

    ValueZV = analogRead(ValueZVPin); 

    lastRunTimeCondition = -cycleTimeState;         // so that you can start with the first start immediately 
    
    initialization ();  

}  

void loop () 
{ 

  switch (state) 
  { 

  case 1:       // open flap  

    Serial.println ("Z1");  

    stopMotor (); 

    
  if (driveTrigger == true || rotaryanglechangevalue == true) 
  { 

    rotaryanglechangevalue = false; 

    state = 2;  

  }  

  break;  

  
  case 2:         // flap closes  

    Serial.println ("Z2");  

    if (rotaryanglevalue <= posSoftStopClose) 
    { 

      setRotationDirection (-1); 

      setPercentageOfBatteryVoltage (SpeedSoftStopPWM); 
    } 

    else 
    { 
      setRotationDirection (-1);  
      setPercentageOfBatteryVoltage (SpeedNormalPWM);  
    }  

    if ((StateZV == 2 || StateZV == 4) && rotaryanglevalue <= posfrontZV) 
    { 
      digitalWrite (relaydrivepin, HIGH);  
    } 

    if (rotaryanglevalue <= posClosed) 
    { 
      lastRunTimeRelay = millis ();  
      state = 3;  
    } 

  break;  

  
  case 3:         // flap locked  

    Serial.println ("Z3");  

    if (millis () - lastRunTimeRelay >= cycleTimeRelay) 
    { 
      Serial.println ("locked"); 
      digitalWrite (relaydrivepin, LOW);  
      lockFailig = true;  
      unlocking = false;  
    }  

    if (lockFailig == true)
    {
      state = 4;
    } 

  break;  

  
  case 4:         // flap closed  

    Serial.println ("Z4"); 

    stopMotor ();
  
    if (controlTrigger == true || StateZV == 1 || StateZV == 3) 
    { 
      rotaryanglechangevalue = false;  
      lastRunTimeRelay = millis ();  
      state = 5;  
    }  

  break;  

  
  case 5:         // flap unlocks 
    Serial.println ("Z5");  

    digitalWrite (relaydrivepin, HIGH);  

    if (millis () - lastRunTimeRelay >= cycleTimeRelay)  
    { 
      Serial.println ("unlocked"); 
      lockFailig = false; 
      unlocking = true;  
    } 

    if (lockFailig == false) 
    {
      state = 6;  
    } 

  break;  

  
  case 6:         // flap opens  

    Serial.println ("Z6");  

    EKS ();  

    rotaryanglechangevalue = false;  

  
    if (rotaryanglevalue >= posfrontZV) 
    { 
      digitalWrite (relaydrivepin, LOW);  
    } 

    if (rotaryanglevalue >= posSoftStopOpen) 
    { 
      setRotationDirection (1);
      setPercentageOfBatteryVoltage (SpeedSoftStopPWM);  
    }
    else
    { 
      setRotationDirection (1);  
      setPercentageOfBatteryVoltage (SpeedNormalPWM);  
    }  

    if (rotaryanglevalue >= posOpen) 
    { 
      lastRunTimeCondition = millis ();  
      state = 1;  
    } 

  break;  

  }  

  knifeversion(); 

} 

  
void initialization () 

{ 

  rotaryanglevalue = analogRead (A0); 

  if (rotaryanglevalue <= posClosed) 
  {                     // the system is already in position Z4 (closed)  
    state = 4;  
  }  

  else if (rotaryanglevalue >= posOpen) 
  {                     // the system is already in position Z1 (open)  
    state = 1;  
  } 

  else 
    {                     // If the system is not in an end position  
  
      if (rotaryanglevalue> posSystemMitte) 
      {                     // the system is in the upper range  
        initCondition = 1; 
  
        while (rotaryanglevalue <= posOpen) 
        { 
          rotaryanglevalue = analogRead (A0); 
  
          Serial.println ("Open Start INIT"); 
  
          Serial.println (rotaryanglevalue); 
  
          if (rotaryanglevalue >= posSoftStopOpen) 
          { 
  
            setRotationDirection (1); 
  
            setPercentageOfBatteryVoltage (SpeedSoftStopPWM); 
  
          } 
  
          else 
          { 
  
            setRotationDirection (1); 
  
            setPercentageOfBatteryVoltage (SpeedNormalPWM); 
  
          } 
        }
       

    lastRunTimeCondition = millis (); 

    unlocking = true; 

    lockFailig = false; 

    rotaryanglechangevalue = rotaryanglevalue; 

    state = 1; 

    initCondition = 0; 

    } 

  else if (rotaryanglevalue <= posSystemMitte) 
  {                             // is the system at the bottom 

    initCondition = 2; 

    while (rotaryanglevalue > posClosed || initFinished == LOW) 
    { 

      Serial.print ("Start INIT close"); 

      Serial.println (digitalread (relaycontrolspin)); 

      rotaryanglevalue = analogRead (A0); 

      Serial.println (rotaryanglevalue); 

      if (TurningangleValue <= ItemSoftStopClose) 
      {  

        setRotationDirection(-1); 

        setPercentageOfBatteryVoltage(SpeedSoftStopPWM); 

      } 

      else 
      {  

        setRotationDirection(-1); 

        setPercentageOfBatteryVoltage(SpeedNormalPWM); 

      } 

      if (TurningangleValue <= ItemfrontZV) 
      { 

        digitalWrite(relayOn-ControlPin, HIGH);  

      } 

      if (initState == 2) 
      { 

        if (TurningangleValue <= ItemClosed ) 
        { 

          lastRunTimeRelay = millis();  

          initState = 21; 

        } 

      } 

      if (initState == 21 || initState == 22) 
      {  

        if (millis() - lastRunTimeRelay >= cycleTimeRelay) 
        {  

          Serial.println("Locked");  

          digitalWrite(relayOn-ControlPin, LOW); 

          initState = 22;  

        } 

      } 

      if (initState == 22 && digitalRead(relayOn-ControlPin) == 0) 
      { 

        initFinished = HIGH;  

      } 
    
    } 

    lastRunTimeState = millis();  
  
    unlockingready = false;  

    lockingready = true; 

    rotaryangleChangeValue = TurningangleValue; 

    State = 4; 

    initState = 0;  

  } 

} 
  

void knifeversion() 
{  

  valueZV = analogRead(valueZVPin); 

  on-controlTrigger = digitalRead(12); 

  TurningangleValue = analogRead(A0); 

  Serial.println(TurningangleValue); 

                            //Serial.println(ElectricitySensorValue); 

                            // Serial.println(StateZV); 

  if ((State != 1 && State != 4) || State == 6) 
  {  

    rotaryanglechangevalue = TurningangleValue; 

  } 

  if (millis() - lastRunTimeState >= cycleTimeState) 
  { 

    CaptureTurningangleChange(); 

  } 

  LockingUnlockingZV(); 

  Tothecurrentcalculation(); 

  EKS();  

} 

//*************************************MotorControl*******************************************//  

void setMotorPin (int MOTOR_PWM_PIN, int MOTOR_IN_A, int MOTOR_IN_B) 
{ 

  TCCR1B = TCCR1B & 0b11111000 | 0x01;  

  pwmPin = MOTOR_PWM_PIN; 

  inA = MOTOR_IN_A; 

  inB = MOTOR_IN_B;  

  pinMode(pwmPin, OUTPUT); 

  pinMode(inA, OUTPUT);  

  pinMode(inB, OUTPUT);  

  digitalWrite(inA, LOW);  

  digitalWrite(inB, LOW); 

}  


void setRotationDirection(int8_t motorDirection) 
{  

  switch (motorDirection) 
  {  

  case -1: digitalWrite(inA, HIGH); digitalWrite(inB, LOW); rotationDirection = -1;  

  break;
  

  case 1: digitalWrite(inA, LOW); digitalWrite(inB, HIGH); rotationDirection = 1;  

  break;
  

  default: 

    Serial.println("Error Motor Stop");  

    stopMotor(); 

  break; 

  } 

} 


void stopMotor(void) 
{ 

  digitalWrite(inA, HIGH); digitalWrite(inB, HIGH);  

} 


void setPercentageOfBatteryVoltage(int percent) 
{ 

  driveVoltagePercentage = percent; 

  analogWrite(pwmPin, percent); 

} 


//*************************************rotaryanglesensor*******************************************//  

void VersionrotaryangleChange() 
{ 

  if (millis() - lastRunTime >= cycleTime) 
  { 

    rotaryanglechangevalue = TurningangleValue; 

    TurningangleChange = false; 

    lastRunTime = millis();  

  } 

  if ((abs(rotaryanglechangevalue - TurningangleValue )) >= 10) 
  { 

    TurningangleChange = true; 

  } 

  Else 
  { 

    TurningangleChange = false; 

  } 

                          //Serial.println(TurningangleChange); 

} 


//*************************************ZV*********************************************************// 

void LockingUnlockingZV() {  

// SWB = HIGH SWA = LOW --> Value is the same 376 |  

// SWB = LOW SWA = LOW --> Value is the same 484 |--> (376 + 484)/2 = 430 

// SWB = HIGH SWA = HIGH --> Value is the same 596 |--> (484 + 596)/2 = 540 

// SWB = LOW SWA = HIGH --> Value is the same 878 |--> (596 + 878)/2 = 737 

  if (ValueZV > 230 && ValueZV <= 430 ) 
  { 

    StateZV = 1;  

  } 

  else if (ValueZV > 430 && ValueZV <= 540) 
  {  

    StateZV = 2;  

  } 

  else if (ValueZV > 540 && ValueZV <= 737) 
  {  

    StateZV = 3; 

  } 
  else if (ValueZV > 737 && ValueZV <= 937) 
  { 

    StateZV = 4; 

  } 
  else 
  { 

    Serial.println("ERROR ZV"); 

  } 

} 


//******************************************CS*****************************************************//  

void CurrentCalculation() 
{  

  CurrentSensorValue = ((analogRead(CurrentSensorPin) * 4.8) / (1023 * 0.14)); 

  Serial.println(CurrentSensorValue);  

} 

void EKS() 
{ 

  if (CurrentSensorValue > ClampingProtectionrefField && TurningangleValue <= ItemOpens && TurningangleValue >= ItemClosed ) 
  {  

    digitalWrite(relayOn-ControlPin, LOW);  

    stopMotor();  

    Serial.println("EKS");  

    if (State == 2) 
    {  

      while (TurningangleValue <= ItemOpen) 
      {  

        Serial.println("EKS open"); 

        TurningangleValue = analogRead(A0);  

        Serial.println(TurningangleValue);  

        if (turningangleValue >= ItemSoftStopOpen) 
        {  

          setRotationDirection(1);  

          setPercentageOfBatteryVoltage(SpeedSoftStopPWM);  

        } 

        else 
        { 
          setRotationDirection(1);  

          setPercentageOfBatteryVoltage(SpeedNormalPWM); 
        } 

      }  

      lastRunTimeState = millis(); 

      State = 1; 

    } 

  else if (State == 6) 
  { 

      while (LockingReady == false) 
      { 

        Serial.println("EKS Close"); 

        TurningangleValue = analogRead(A0); 

        Serial.println(TurningangleValue);  

        if (TurningangleValue <= ItemSoftStopClose ) 
        {  

          setRotationDirection(-1);  

          setPercentageOfBatteryVoltage(SpeedSoftStopPWM);  

        } 

        else 
        { 

          setRotationDirection(-1); 

          setPercentageOfBatteryVoltage(SpeedNormalPWM); 

        } 

        if (TurningangleValue <= ItemBeforeZV) 
        { 

          digitalWrite(relayOn-ControlPin, HIGH); 

        } 

        if ((StateZV == 2 || StateZV == 4) && TurningangleValue <= ItemClosed ) 
        { 

          if (LockingCounterEKS == 0) 
          { 

            lastRunTimeRelay = millis(); 

            LockingCounterEKS++;  

          } 

          if (((millis() - lastRunTimeRelay) >= cycleTimeRelay) && (LockingCounterEKS != 0)) 
          { 

            Serial.println("Locked"); 

            digitalWrite(relayOn-ControlPin, LOW); 

            LockingReady = true; 

            UnlockingReady = false; 

          } 

        } 

        Serial.println(LockingReady); 

        Serial.println(LockingCounterEKS); 

      } 

    digitalWrite(relayOn-ControlPin, LOW); 

    lastRunTimeState = millis(); 

    LockingCounterEKS = 0; 

    State = 4; 

  } 

  } 

} 
