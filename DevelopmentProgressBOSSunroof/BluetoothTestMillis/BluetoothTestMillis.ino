// Ryan Arnoldy 
// 6/10/2020 
// Implement Millis timing function for reading Bluetooth 

#define LED2 7 
/*Timing Vars*/
const unsigned long event1 =1200 ; // Read Bluetooth 
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

// Changed for serial monitor testing
String bluetooth() {
  if (Serial2.available()) {
    String in = Serial2.readString();
    return in;
  }
}


void selection(String sel) {
	int Va1; // Not sure if needed  
	int Va2; 
  // Testing push button inputs  	
  if (sel.startsWith("TST1")) {
    Test1();  
    loop();
  }
  else if (sel.startsWith("TST2")) {
    Test2(); 
    loop(); 
  }
  // Test text box input
  else if (sel.startsWith("Va1")){
  	Input1();  
  }
  else if (sel.startsWith("Va2")){
  	Input2();
  }


}




// Button input functions  
void Test1(){
	Serial.print("test1");
	loop(); 
}
void Test2(){
	Serial.print("test2"); 
	loop(); 

}


int Input1(){
	Serial.print("Input1: ");
  	String sel_Va1 = sel.substring(3,sel.length());  // strip prefix 
  	Va1 = sel_Va1.toInt(); // convert to int     
  	Serial.print(Va1);  
}
int Input2(){
	else if (sel.startsWith("Va2")){
  	Serial.print("Input2: ");
  	String sel_Va2 = sel.substring(3,sel.length()); 
  	Va2 = sel_Va2.toInt();  
  	Serial.print(Va2);  
} 