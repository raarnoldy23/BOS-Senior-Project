
#include <HallCounter.h>
#include <TimerOne.h>

// Constructor

HallCounter::HallCounter(int HallPin_1, int HallPin_2, int AmountPoles) {
	
    initialize();

	hallPin_1   = HallPin_1;
	hallPin_2	= HallPin_2;+
	
	
	amountPoles = AmountPoles;

	pinMode(hallPin_1, INPUT);
	pinMode(hallPin_2, INPUT);

	attachInterrupt(digitalPinToInterrupt(hallPin_1), int_hall_1, CHANGE);

}

void HallCounter::initialize(void) {	
	last_RisingEdge = 0;
	secondLast_RisingEdge = 0;
	last_FallingEdge = 0;
	secondLast_FallingEdge = 0;

	lastPulsWidth_t1 = 0;
    lastPulsWidth_t2 = 0;
    
	currentPulsWidth = 0;
	flankTime = 0;
    
    currentRotationSpeed = 0;    
}

// get the current rotation speed in rps

float HallCounter::getRotationSpeed(void){

	currentTime = micros();

	if(hallpin_1_state){
		lastPulsWidth_t1= last_RisingEdge - last_FallingEdge;
		currentPulsWidth_t1 = currentTime - last_RisingEdge;
	} else {
		lastPulsWidth_t2 = last_FallingEdge - last_RisingEdge;
		currentPulsWidth_t2 = currentTime - last_FallingEdge;
	}
	
    if(currentPulsWidth_t1 > lastPulsWidth_t1){
        currentRotationSpeed=float(2000000.0/(amountPoles*(currentPulsWidth_t1+lastPulsWidth_t2)));
	}
    else if(currentPulsWidth_t2 > lastPulsWidth_t2) {
        currentRotationSpeed=float(2000000.0/(amountPoles*(lastPulsWidth_t1+currentPulsWidth_t2)));
    }
    else {
        currentRotationSpeed=float(2000000.0/(amountPoles*(lastPulsWidth_t1+lastPulsWidth_t2)));
    }
    
    if(currentRotationSpeed < 20){
		return 0;
	} else {
		return currentRotationSpeed;    
	}
}

void HallCounter::resetHallCounter(int reset){
    
    if(reset==1){ 
        puls_count_1 = 0;
    }
}

// Interrupt Handler
void HallCounter::int_hall_1() {

	flankTime = micros();
    
	hallpin_1_state = digitalRead(hallPin_1);
	hallpin_2_state = digitalRead(hallPin_2);
    
	calcRotationDirection_1();

	if(hallpin_1_state){
		secondLast_RisingEdge = last_RisingEdge;
		last_RisingEdge = flankTime;         
	} else {
		secondLast_FallingEdge = last_FallingEdge;
		last_FallingEdge = flankTime;         
	}

    switch(rotationDirection){
		case -1: puls_count_1--; break;
		case 1: puls_count_1++; break;
	}
}
void HallCounter::calcRotationDirection_1() {

    if(hallpin_1_state){
 		if(hallpin_2_state){
			rotationDirection = 1;
		} else {
			rotationDirection = -1;
		}
	} else {
		if(hallpin_2_state){
			rotationDirection = -1;
		} else {
			rotationDirection = 1;
		}
	}	
}

long HallCounter::getPulsCount_1(void){
	return puls_count_1; // returns the amount of rising AND falling edges for Hallsensor 1
}

int HallCounter::getArrayIndex(void){
    return arrayIndex;
}

float HallCounter::getRefCurveArray(int index){
     return refCurveArray[index];
}

int HallCounter::hallPin_1 = -1;
int HallCounter::hallPin_2 = -1;
int HallCounter::amountPoles = 0;


long HallCounter::last_RisingEdge=0;
long HallCounter::secondLast_RisingEdge=0;
long HallCounter::last_FallingEdge=0;
long HallCounter::secondLast_FallingEdge=0;

long HallCounter::lastPulsWidth_t1=0;
long HallCounter::lastPulsWidth_t2=0;
long HallCounter::currentPulsWidth=0;
long HallCounter::currentPulsWidth_t1=0;
long HallCounter::currentPulsWidth_t2=0;
long HallCounter::flankTime=0;

boolean HallCounter::hallpin_1_state;
boolean HallCounter::hallpin_2_state;
long HallCounter::puls_count_1=0;
int HallCounter::arrayIndex=0;
float HallCounter::refCurveArray[200]={0};
long HallCounter::currentTime=0;
float HallCounter::currentRotationSpeed=0;
int8_t HallCounter::rotationDirection=1;

