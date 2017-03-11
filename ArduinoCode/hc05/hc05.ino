/*
Arduino Model: UNO
*/

#include <SoftwareSerial.h>

const int hc05_rx_pin = 11; // Connect to HC05's tx
const int hc05_tx_pin = 12; // Connect to HC05's rx

String read_string;

SoftwareSerial hc05(hc05_rx_pin, hc05_tx_pin);

const int ledpin = 13;

void setup() {
	Serial.begin(9600);
	hc05.begin(9600);
	pinMode(ledpin, OUTPUT);
}

void loop() {
	read_string = "";
	while (hc05.available() > 0) {
		char c = hc05.read();
		read_string += c;
	}
	Serial.println(read_string);
	delay(1000);
}