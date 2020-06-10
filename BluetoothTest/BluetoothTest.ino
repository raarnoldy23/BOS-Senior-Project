// Test the function of bluetooth module
// Sending and returning data over bluetooth
// Implement Millis function

/*Timing variables*/
const unsigned long ReadBluetooth = 2500; // read bluetooth frequency 
unsigned long lastbluetoothread = 0; // place holder 
const unsigned long selection = 1000;  
unsigned long lastselectionread = 0;    

void setup() {
  Serial.begin(9600); //SM
  Serial2.begin(9600); //Bluetooth
  Serial.print("begin..");
}


void loop() {
  unsigned long currentTime = millis(); // clock  
  String input; //holds user input 
  /*Timing functions*/
  if (currentTime - lastbluetoothread >= ReadBluetooth)
    input = bluetooth();
    lastbluetoothread = currentTime; // reset last read clock 
  }


  input = bluetooth();
  selection(input);
} 


// Function the initalizes bluetooth connection
String bluetooth() {
  if (Serial2.available()) {
    Serial.print("Reading");
    String in = Serial2.readString();
    return in;
  }
}


// Function that assigns selection to appropriate sub-function
void selection(String sel) {
  if (sel.startsWith("TST1")) {
    Serial.println("test1");
  }

  else if (sel.startsWith("TST2")) {
    Serial.println("test2");
  }
  else {
    Serial.println("No valid input");
    bluetooth();
  }

}
