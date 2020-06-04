// Test the function of bluetooth module
// Sending and returning data over bluetooth
#define led 13

void setup() {
  Serial.begin(9600); //SM
  Serial2.begin(9600); //Bluetooth
  Serial.print("begin..");
}


void loop() {
  String  input = bluetooth();
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
