//++++++++++++++++++++++++++++++++++++++++ Einstellungen (einstellbar) ++++++++++++++++++++++++++++++++++++++++//

//------------------------------ VAR einstellbar ------------------------------//
int posGeschlossen = 130;
int posGeoeffnet = 770;
int posSoftStopSchliessen = (posGeschlossen + 50);
int posSoftStopOeffnen = (posGeoeffnet - 50);
int posVorZV = posGeschlossen + 25 ;
int geschwindigkeitNormalPWM = 150;
int geschwindigkeitSoftStopPWM = 120;
float einklemmschutzRefFeld = 0.70;
//-----------------------------------------------------------------------------//

//++++++++++++++++++++++++++++++++++++++++++ Einstellungen (fixiert) ++++++++++++++++++++++++++++++++++++++++++//

//---------------------------- VAR Systemposition -----------------------------//
int posSystemMitte = (posGeoeffnet + posGeschlossen) / 2;

//-----------------------------------------------------------------------------//

//---------------------------- VAR Zustandsautomat ----------------------------//
int zustand = 0; // Aktueller Zustand des Systems
unsigned long lastRunTimeZustand = 0;
unsigned long cycleTimeZustand = 1500;
//-----------------------------------------------------------------------------//

//---------------------------- VAR Drehwinckelsensorlogik ---------------------//
unsigned long lastRunTime = 0;
unsigned long cycleTime = 1000;
int drehwinckelWert = 0; // Werte des Drehwinckelsensors max:900  min: 150
int drehwinckelAenderungWert = 0; //
bool drehwinckelAenderung = false; // Hat sich der drewinckel geaendert
//-----------------------------------------------------------------------------//

//---------------------------- VAR ZV-Steller ---------------------------------//
bool verriegelungFertig = false; // Verriegelungsprozedur beendet
bool entriegelungFertig = false; // Entriegelungsprozedur beendet
int zustandZV = 0; // Entlagentaste beim ZV Modul
int relayAnsteuerungsPin = 7; //Das Relay steuert die ZV
int wertZVPin = A4;// Pin ZV
int wertZV = 0;// Werte der ZV
bool ZV800 = false;
bool ZV10 = false;
unsigned long lastRunTimeZV = 0;
//unsigned long cycleTimeZV = 200;
unsigned long lastRunTimeRelay = 0;
unsigned long cycleTimeRelay = 200;
//-----------------------------------------------------------------------------//

//---------------------------- VAR Useransteuerung ----------------------------//
int ansteuerungsPin = 12;
bool ansteuerungsTrigger = false; // Tastendruck vom benutzer
//-----------------------------------------------------------------------------//

//---------------------------- VAR Motorparameter -----------------------------//
int pwmPin;
int inA;
int inB;
int rotationDirection;
int driveVoltagePercentage;
//-----------------------------------------------------------------------------//

//---------------------------- VAR Stromsensor --------------------------------//
int stromSensorPin = A6;
double stromSensorWert = 0;
//-----------------------------------------------------------------------------//

//---------------------------- VAR EKS ----------------------------------------//
int verriegelungZaelerEKS = 0;
//-----------------------------------------------------------------------------//

//---------------------------- VAR Initialisierung ----------------------------//
int initZustand = 0;
bool initFertig = LOW;
//-----------------------------------------------------------------------------//

void setup() {

  Serial.begin(9600);
  
  pinMode(ansteuerungsTrigger, INPUT);
  pinMode(drehwinckelWert, INPUT);
  pinMode(stromSensorPin, INPUT);
  pinMode(wertZVPin, INPUT);
  pinMode(relayAnsteuerungsPin, OUTPUT);
  pinMode(wertZVPin, INPUT);
  
  setMotorPin (9, 5, 4);
  drehwinckelWert = analogRead(A0);
  drehwinckelAenderungWert = drehwinckelWert;
  ansteuerungsTrigger = digitalRead(ansteuerungsPin);
  wertZV = analogRead(wertZVPin);

  lastRunTimeZustand = -cycleTimeZustand; // damit man beim ersten start sofort ansteuern kann

  initialisierung();
}


void loop() {

  switch (zustand) {
    case 1: // Klappe geoeffnet
      Serial.println("Z1");

      stopMotor();

      if (ansteuerungsTrigger == true || drehwinckelAenderung == true) {
        drehwinckelAenderung = false;
        zustand = 2;
      }
      break;

    case 2: // Klappe schliesst
      Serial.println("Z2");

      if (drehwinckelWert <= posSoftStopSchliessen) {
        setRotationDirection(-1);
        setPercentageOfBatteryVoltage(geschwindigkeitSoftStopPWM);
      }
      else {
        setRotationDirection(-1);
        setPercentageOfBatteryVoltage(geschwindigkeitNormalPWM);
      }

      if ((zustandZV == 2 || zustandZV == 4) && drehwinckelWert <= posVorZV) {
        digitalWrite(relayAnsteuerungsPin, HIGH);
      }

      if ( drehwinckelWert <= posGeschlossen ) {
        lastRunTimeRelay = millis();
        zustand = 3;
      }

      break;

    case 3: // Klappe verriegelt
      Serial.println("Z3");
      if (millis() - lastRunTimeRelay >= cycleTimeRelay) {
        Serial.println("verriegelt");
        digitalWrite(relayAnsteuerungsPin, LOW);
        verriegelungFertig = true;
        entriegelungFertig = false;

      }
      if (verriegelungFertig == true) {
        zustand = 4;
      }

      break;

    case 4: // Klappe geschlossen
      Serial.println("Z4");

      stopMotor();

      if (ansteuerungsTrigger == true || zustandZV == 1 || zustandZV == 3  ) {
        drehwinckelAenderung = false;
        lastRunTimeRelay = millis();

        zustand = 5;

      }

      break;

    case 5: // Klappe entriegelt
      Serial.println("Z5");

      digitalWrite(relayAnsteuerungsPin, HIGH);
      if (millis() - lastRunTimeRelay >= cycleTimeRelay) {
        Serial.println("entriegelt");
        verriegelungFertig = false;
        entriegelungFertig = true;   
      }
       if (verriegelungFertig == false) {
        zustand = 6;
      }

      break;

    case 6: // Klappe oeffnet
      Serial.println("Z6");
      EKS();
      drehwinckelAenderung = false;

      if (drehwinckelWert >= posVorZV) {
        digitalWrite(relayAnsteuerungsPin, LOW);
        
      }

      if ( drehwinckelWert >= posSoftStopOeffnen ) {
        setRotationDirection(1);
        setPercentageOfBatteryVoltage(geschwindigkeitSoftStopPWM);
      }
      else {
        setRotationDirection(1);
        setPercentageOfBatteryVoltage(geschwindigkeitNormalPWM);
      }

      if (drehwinckelWert >= posGeoeffnet) {
        lastRunTimeZustand = millis();
        zustand = 1;
      }

      break;
  }

  messerfassung();

}

void initialisierung() {

  drehwinckelWert = analogRead(A0);

  if (drehwinckelWert <= posGeschlossen) { // ist das System schon in Position Z4(geschlossen)
    zustand = 4;
  }
  else if (drehwinckelWert >= posGeoeffnet) { // ist das System schon in Position Z1 (geöffnet)
    zustand = 1;
  }
  else { // Wenn das System in keiner Endposition ist

    if ( drehwinckelWert > posSystemMitte) { // ist das System im oberen Bereich
      initZustand = 1;
      while ( drehwinckelWert <= posGeoeffnet) {       
        drehwinckelWert = analogRead(A0);
        Serial.println("Start INIT oeffnen");        
        Serial.println(drehwinckelWert);
        if ( drehwinckelWert >= posSoftStopOeffnen) {
          setRotationDirection(1);
          setPercentageOfBatteryVoltage(geschwindigkeitSoftStopPWM);
        }
        else {
          setRotationDirection(1);
          setPercentageOfBatteryVoltage(geschwindigkeitNormalPWM);
        }

      }
      lastRunTimeZustand = millis();
      entriegelungFertig = true;
      verriegelungFertig = false;
      drehwinckelAenderungWert = drehwinckelWert;
      zustand = 1 ;
      initZustand = 0;

    }
    else if ( drehwinckelWert <= posSystemMitte) { // ist das System im unteren Bereich
      initZustand = 2;
      while ( drehwinckelWert > posGeschlossen || initFertig == LOW ) {
        Serial.print("Start INIT schliessen ");
        Serial.println(digitalRead(relayAnsteuerungsPin));
        drehwinckelWert = analogRead(A0);
        Serial.println(drehwinckelWert);

        if (drehwinckelWert <= posSoftStopSchliessen) {

          setRotationDirection(-1);
          setPercentageOfBatteryVoltage(geschwindigkeitSoftStopPWM);
        }
        else {

          setRotationDirection(-1);
          setPercentageOfBatteryVoltage(geschwindigkeitNormalPWM);
        }

        if (drehwinckelWert <= posVorZV) {
          digitalWrite(relayAnsteuerungsPin, HIGH);
        }
        if (initZustand == 2) {

          if ( drehwinckelWert <= posGeschlossen ) {
            lastRunTimeRelay = millis();
            initZustand = 21;
          }
        }
        if (initZustand == 21 || initZustand == 22) {
          if (millis() - lastRunTimeRelay >= cycleTimeRelay) {
            Serial.println("verriegelt");
            digitalWrite(relayAnsteuerungsPin, LOW);
            initZustand = 22;
          }
        }
        if (initZustand == 22 && digitalRead(relayAnsteuerungsPin) == 0) {
          initFertig = HIGH;
        }
      }

      lastRunTimeZustand = millis();
      entriegelungFertig = false;
      verriegelungFertig = true;
      drehwinckelAenderungWert = drehwinckelWert;
      zustand = 4 ;
      initZustand = 0;
    }
  }
}

void messerfassung() {

  wertZV = analogRead(wertZVPin);
  ansteuerungsTrigger = digitalRead(12);
  drehwinckelWert = analogRead(A0);
  Serial.println(drehwinckelWert);
  //Serial.println(stromSensorWert);
  // Serial.println(zustandZV);
  if ((zustand != 1 && zustand != 4) || zustand == 6) {
    drehwinckelAenderungWert = drehwinckelWert;
  }

  if (millis() - lastRunTimeZustand >= cycleTimeZustand) {
    erfassungDrehwinckelAenderung();
  }
  VerriegelungEntriegelungZV();
  stromBerechnung();

  EKS();

}

//*************************************Motorsteuerung*******************************************//

void setMotorPin (int MOTOR_PWM_PIN, int MOTOR_IN_A, int MOTOR_IN_B) {

  TCCR1B = TCCR1B & 0b11111000 | 0x01;

  pwmPin = MOTOR_PWM_PIN;
  inA = MOTOR_IN_A;
  inB  = MOTOR_IN_B;

  pinMode(pwmPin, OUTPUT);
  pinMode(inA, OUTPUT);
  pinMode(inB, OUTPUT);

  digitalWrite(inA, LOW);
  digitalWrite(inB, LOW);

}

void setRotationDirection(int8_t motorDirection) {
  switch (motorDirection) {
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

void stopMotor(void) {
  digitalWrite(inA, HIGH); digitalWrite(inB, HIGH);
}

void setPercentageOfBatteryVoltage(int percent) {
  driveVoltagePercentage = percent;
  analogWrite(pwmPin, percent);
}

//*************************************Drehwinckelsensor*******************************************//

void erfassungDrehwinckelAenderung() {

  if (millis() - lastRunTime >= cycleTime) {

    drehwinckelAenderungWert = drehwinckelWert;

    drehwinckelAenderung = false;

    lastRunTime = millis();
  }

  if ((abs(drehwinckelAenderungWert - drehwinckelWert )) >= 10) {

    drehwinckelAenderung = true;

  }
  else {

    drehwinckelAenderung = false;

  }

  //Serial.println(drehwinckelAenderung);

}

//*************************************ZV*********************************************************//

void VerriegelungEntriegelungZV() {

  // SWB = HIGH  SWA = LOW  --> Wert ist gleich 376 |
  // SWB = LOW   SWA = LOW  --> Wert ist gleich 484 |--> (376 + 484)/2 = 430
  // SWB = HIGH  SWA = HIGH --> Wert ist gleich 596 |--> (484 + 596)/2 = 540
  // SWB = LOW   SWA = HIGH --> Wert ist gleich 878 |--> (596 + 878)/2 = 737


  if (wertZV > 230 && wertZV <= 430 ) {
    zustandZV = 1;
  }
  else if (wertZV > 430 && wertZV <= 540) {
    zustandZV = 2;
  }
  else if (wertZV > 540 && wertZV <= 737) {
    zustandZV = 3;
  }
  else if (wertZV > 737 && wertZV <= 937) {
    zustandZV = 4;
  }
  else {
    Serial.println("ERROR ZV");
  }
}

//******************************************CS*****************************************************//

void stromBerechnung() {
  stromSensorWert  = ((analogRead(stromSensorPin) * 4.8) / (1023 * 0.14));
  Serial.println(stromSensorWert);
}

void EKS() {

  if (stromSensorWert > einklemmschutzRefFeld &&  drehwinckelWert <= posGeoeffnet && drehwinckelWert >= posGeschlossen  ) {
    digitalWrite(relayAnsteuerungsPin, LOW);
    stopMotor();
    Serial.println("EKS");

    if (zustand == 2) {

      while ( drehwinckelWert <= posGeoeffnet) {
        Serial.println("EKS oeffnen");
        drehwinckelWert = analogRead(A0);
        Serial.println(drehwinckelWert);

        if ( drehwinckelWert >= posSoftStopOeffnen) {
          setRotationDirection(1);
          setPercentageOfBatteryVoltage(geschwindigkeitSoftStopPWM);
        }
        else {
          setRotationDirection(1);
          setPercentageOfBatteryVoltage(geschwindigkeitNormalPWM);
        }

      }
      lastRunTimeZustand = millis();
      zustand = 1 ;
    }
    else if (zustand == 6) {
      while (verriegelungFertig == false) {

        Serial.println("EKS schliessen");
        drehwinckelWert = analogRead(A0);
        Serial.println(drehwinckelWert);


        if (drehwinckelWert <= posSoftStopSchliessen ) {
          setRotationDirection(-1);
          setPercentageOfBatteryVoltage(geschwindigkeitSoftStopPWM);
        }
        else {
          setRotationDirection(-1);
          setPercentageOfBatteryVoltage(geschwindigkeitNormalPWM);
        }

        if (drehwinckelWert <= posVorZV) {
          digitalWrite(relayAnsteuerungsPin, HIGH);
        }

        if ((zustandZV == 2 || zustandZV == 4) && drehwinckelWert <= posGeschlossen ) {

          if (verriegelungZaelerEKS == 0) {
            lastRunTimeRelay = millis();
            verriegelungZaelerEKS++;
          }
          if (((millis() - lastRunTimeRelay) >= cycleTimeRelay) && (verriegelungZaelerEKS != 0)) {
            Serial.println("verriegelt");
            digitalWrite(relayAnsteuerungsPin, LOW);
            verriegelungFertig = true;
            entriegelungFertig = false;
          }

        }
        Serial.println(verriegelungFertig);
        Serial.println(verriegelungZaelerEKS);

      }
      digitalWrite(relayAnsteuerungsPin, LOW);
      lastRunTimeZustand = millis();
      verriegelungZaelerEKS = 0;
      zustand = 4 ;
    }


  }
}
