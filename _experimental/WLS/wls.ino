/*
Water level sensor using PNP transistors and pull-up resistors
Floating signals are filtered using queue data structure 
*lb ~ lower bound; ub ~ upper bound;
*/

#include <QueueArray.h>

// WLS input pins
const int wls_lb_pin = 4;
const int wls_ub_pin = 6;

// Digital input value
int wls_lb_val; // Signal: LOW = circuit is closed
int wls_ub_val;

// Digital inputs are converted to signals of 0 and 1
// 0 ~ circuit is opened; 1 ~ circuit is closed
int wls_lb_sig;
int wls_ub_sig;

int wls_waterlevel; // waterlevel = -1 + lb_state + ub_state
					// -1: below lb; 0: between lb and ub; 1: above ub

boolean wls_start = false;
unsigned long wls_starttime;

void setup() {
	// Initializing Serial object
	Serial.begin(9600);

	// Setting up sensor pins with pull-up resistors
	pinMode(wls_lb_pin, INPUT_PULLUP);
	pinMode(wls_ub_pin, INPUT_PULLUP);
	digitalWrite(wls_lb_pin, HIGH);
	digitalWrite(wls_ub_pin, HIGH);
}

// Floating signal filter
void wls_control() {

	static boolean wls_firsttime = true;
	// A queue for each sensor
	static QueueArray <int> wls_lb_sigqueue;
	static QueueArray <int> wls_ub_sigqueue;

	// Setting variables to count LOW signals from the input pins
	static int wls_lb_lsigcnt = 0;
	static int wls_ub_lsigcnt = 0;

	// Reading sensor 
	int wls_lb_val = digitalRead(wls_lb_pin);
	int wls_ub_val = digitalRead(wls_ub_pin);

	// Appending signals to the queue
	wls_lb_sigqueue.enqueue(wls_lb_val);
	wls_ub_sigqueue.enqueue(wls_ub_val);
	
	// Counting LOW signals from input pins
	if (wls_lb_val == LOW) {
		wls_lb_lsigcnt++;
	}
	if (wls_ub_val == LOW) {
		wls_ub_lsigcnt++;
	}

	// For the filter to start, the first 100 signals must be appended to each queue 
	// At the same time, the pump should be inactive
	if (wls_lb_sigqueue.count() < 100) {
		wls_start = false;
	}
	// Then the filter can start
	else {
		wls_start = true;
		if (wls_firsttime == true) {
			wls_starttime = millis();
			wls_firsttime = false;		
		}
		// Setting accuracy rate at >30/100 signals
		if (wls_lb_lsigcnt <= 30) {
			wls_lb_sig = 0;
		}
		else {
			wls_lb_sig = 1;
		}

		if (wls_ub_lsigcnt <= 30) {
			wls_ub_sig = 0;
		}
		else {
			wls_ub_sig = 1;
		}

		// Removing the first element of each queue
		if (wls_lb_sigqueue.front() == LOW) {
			wls_lb_lsigcnt--;
		}

		if (wls_ub_sigqueue.front() == LOW) {
			wls_ub_lsigcnt--;
		}

		wls_lb_sigqueue.dequeue();
		wls_ub_sigqueue.dequeue();

		//Returning water level
		wls_waterlevel = -1 + wls_lb_sig + wls_ub_sig;
	}
}

void loop() {
	// Filtering signals and controlling pump
	wls_control();
	
	// Printing out water level
	if (wls_start == true) {
		Serial.println(wls_lb_sig);
		Serial.println(wls_ub_sig);		
		Serial.println(millis() - wls_starttime);
		delay(500);
	}
}