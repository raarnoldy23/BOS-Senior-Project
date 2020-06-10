// Ryan Arnoldy 
// 6/10/2020 
// Implement Millis timing function for reading Bluetooth 

#define LED2 7 
/*Timing Vars*/
const unsigned long event1 = 1200; // Read Bluetooth 
unsigned long prev1 = 0; 
const unsigned long event2 = 1200; // Input Function Call 
unsigned long prev2 = 0;  


void setup(){
	Serial.begin(9600); //SM 
	Serial2.begin(9600); // Bluetooth
	//Serial.println("Begin..");
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(LED2, OUTPUT);
}


void loop(){ 
	unsigned long currentTime = millis(); 
	String input; // holds user input
	/*Event Timing*/
	if (currentTime - prev1 >= event1){
		Serial.println("Event1");
		input = bluetooth(); 
		prev1 = currentTime; // Update previous time 
		
	}
	if (currentTime - prev2 >= event2){
		Serial.println("Event2");  
		selection(input); 
		prev2 = currentTime; 
	}
	
}


String bluetooth() {
  if (Serial2.available()) {
    //Serial.print("Reading");
    String in = Serial2.readString();
    return in;
  }
}


void selection(String sel) {
  if (sel.startsWith("TST1")) {
    Serial.println("test1");
    loop();
  }
  else if (sel.startsWith("TST2")) {
    Serial.println("test2");
    loop(); 
  }
}
