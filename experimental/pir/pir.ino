/* Motion detector using PIR sensor
*/

// LED pin
const int led_pin = 13;

// PIR_OUT pin
const int pir_out_pin = 2;

// Digital input value
int pir_val;

void setup() {
	// Initializing Serial object
	Serial.begin(9600);

	// Setting up PIR_OUT pin
	pinMode(pir_out_pin, INPUT);

	// Setting up LED
	pinMode(led_pin, OUTPUT);
}

void loop() {
	// Reading sensor
	pir_val = digitalRead(pir_out_pin);

	// LED is turned ON or OFF according to PIR's signal 
	digitalWrite(led_pin, pir_val);

	// Printing out result on the monitor
	if (pir_val == HIGH) {
		Serial.println(1);
	}
	else {
		Serial.println(0);
	}
}