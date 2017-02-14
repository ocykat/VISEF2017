/* - TDS Sensor -
+ Arduino Model: MEGA
*/

#include <OneWire.h>
#include <DallasTemperature.h>

// Resistors
const int tds_EC_R_chosen = 1000; // any resistor with the resistance of >300 ohm would work
const int tds_EC_R_pin = 25; // resistance of the pin

// tds_EC pins
const int tds_EC_out_pin = A3;
const int tds_EC_gnd_pin = 32;
const int tds_EC_vcc_pin = 33;

// Calibration constants
const float tds_EC_PPM_conversion = 0.7; // EC to PPM conversion constant
const float tds_EC_tempcoef = 0.019; // Temperature coefficient
const float tds_EC_K = 2.88;

// tds_temp pins
const int tds_onewire_bus = 35;
const int tds_temp_vcc_pin = 36;
const int tds_temp_gnd_pin = 37;

// Initialize libraries' objects
OneWire tds_onewire(tds_onewire_bus);
DallasTemperature tds_temp_sensor(&tds_onewire);

// Solution's temperature
float tds_temp_temperature;

// Solution's electrical conductivity
float tds_EC_ECval;
float tds_EC_EC25val; // reference value at 25 degree Celsius

// Solution's ppm
int tds_EC_ppm;

void setup() {
	// Initialize Serial object
	Serial.begin(9600);
	
	// Set up TDS Temperature sensor pins
	pinMode(tds_temp_vcc_pin, OUTPUT);
	digitalWrite(tds_temp_vcc_pin, HIGH);

	pinMode(tds_temp_gnd_pin, OUTPUT);
	digitalWrite(tds_temp_gnd_pin, LOW);

	// Set up TDS EC sensor pins
	pinMode(tds_EC_vcc_pin, OUTPUT);

	pinMode(tds_EC_gnd_pin, OUTPUT);
	digitalWrite(tds_EC_gnd_pin, LOW);

	pinMode(tds_EC_out_pin, INPUT);

	tds_temp_sensor.begin();
}

void loop() {
	// Read solution's temperature
	tds_temp_sensor.requestTemperatures();
	tds_temp_temperature = tds_temp_sensor.getTempCByIndex(0);

	// Provide tds_EC_vcc_pin with HIGH voltage to start measuring
	// Then stop after measurement has been done
	digitalWrite(tds_EC_vcc_pin, HIGH);
	// Read EC's analog input twice, since first read returns incorrect result
	float tds_EC_V_raw;
	for (int i = 0; i < 2; i++) {
		tds_EC_V_raw = analogRead(tds_EC_out_pin);
	}
	digitalWrite(tds_EC_vcc_pin, LOW);

	// Calculations
	float tds_EC_V_in = 5;
	float tds_EC_V_drop = (tds_EC_V_in * tds_EC_V_raw) / 1024.0;
	float tds_EC_R_solution = (tds_EC_V_drop * tds_EC_R_chosen) / (tds_EC_V_in - tds_EC_V_drop) - tds_EC_R_pin;
	float tds_EC_ECval = 1000 / (tds_EC_R_solution * tds_EC_K);
	float tds_EC_EC25val = tds_EC_ECval / (1 + tds_EC_tempcoef * (tds_temp_temperature - 25.0));
	tds_EC_ppm = (tds_EC_EC25val) * (tds_EC_PPM_conversion * 1000);

	// Print out result on the Serial monitor
	if (tds_EC_ppm < 100) {
		Serial.println("Nong do thap hon yeu cau. Them 3ml!");
  	}
	else if (tds_EC_ppm < 300) {
		Serial.println("Nong do thap hon yeu cau. Them 2ml!");
	}
	else if (tds_EC_ppm < 500) {
		Serial.println("Nong do thap hon yeu cau. Them 1ml!");
	}
	else if (tds_EC_ppm >= 500 && tds_EC_ppm <= 800) {
		Serial.println("Nong do dat yeu cau!");
	}
	else if (tds_EC_ppm > 800) {
		Serial.println("Nong do qua cao!");
	}
}