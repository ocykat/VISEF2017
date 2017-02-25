/*
Arduino model: UNO
*/

// DHT11 library
#include <dht.h>

// DHT object
dht dht11;

// DHT11 out pin
const int dht11_out_pin = 7;

void setup() {
	Serial.begin(9600);
}

void dht11_control() {
	// Printing out temperature on Serial monitor
	dht11.read11(dht11_out_pin);
	Serial.print("Nhiet do: ");
	Serial.print(dht11.temperature);
	Serial.print((char)167);
	Serial.println("C");
	
	// Checking if the temperature is in the optimum range: 19 => 24 Celcius
	if (dht11.temperature < 19) {
		Serial.println("Nhiet do thap hon muc tuong cho cay!");
	}
	else if (dht11.temperature > 24) {
		Serial.println("Nhiet do cao hon muc ly tuong cho cay!");
	}
	else {
		Serial.println("Nhiet do dam bao!")
	}

	// Printing out temperature on Serial monitor
	Serial.print("Humidity:");
	Serial.print(dht11.humidity);
	Serial.println((char)37);

	// Checking if the humidity is in the optimum range: >40%
	if (dht11.humidity <= 40) {
		Serial.println("Do am thap hon muc ly tuong cho cay!");
	}
	else {
		Serial.println("Do am dam bao!");
	}

	// Wait for 2 seconds before reading new signal
	delay(2000);
}

void loop() {
	dht11_control();
}