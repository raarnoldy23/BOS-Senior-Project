
#ifndef HallCounter_h
#define HallCounter_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif

class HallCounter {

private:

	//Private variables
	static int hallPin_1;
	static int hallPin_2;
	static int amountPoles;

	// Pulse flanks
	static long last_RisingEdge;
	static long secondLast_RisingEdge;
	static long last_FallingEdge;
	static long secondLast_FallingEdge;
	static long lastPulsWidth_t1;
    static long lastPulsWidth_t2;
    static long lastPulsWidth;
	static long currentPulsWidth;
    static long currentPulsWidth_t1;
    static long currentPulsWidth_t2;
	static long flankTime;

	static long puls_count_1;
    static int arrayIndex;
    static float refCurveArray[200];

	static long currentTime;

	static float currentRotationSpeed;
	static int8_t rotationDirection;

	static boolean hallpin_1_state;
	static boolean hallpin_2_state;

	//Private functions
	void static initialize();
	void static calcRotationDirection_1();
	void static calcRotationDirection_2();

public:	
	// Public Functions
	HallCounter(int HallPin_1, int HallPin_2, int AmountPoles);
	
	static float getRotationSpeed(void);
    static void resetHallCounter(int reset);
	static long getPulsCount_1(void);  
	static void int_hall_1();
    static int getArrayIndex(void);
    static float getRefCurveArray(int index);
 };

#endif

