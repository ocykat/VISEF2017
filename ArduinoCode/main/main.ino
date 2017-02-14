/*
Thuc Hanh High school - HCMUE - Vietnam
Science research project: Household Aerophonics System 
*/

// LED Relays
const int ledRelay_vcc = ;

void ledRelay_setup() {

}

// PIR
const int pir_vcc_pin = 24;
const int pir_out_pin = 26;
const int pir_gnd_pin = 28;
const int pir_led_pin = ;

void pir_setup() {
	pinMode(pir_out_pin, INPUT);
	pinMode(pir_led_pin, OUTPUT);
}

void pir_control() {
	digitalWrite(pir_led_pin, digitalRead(pir_out_pin));
}

// WLS
#include <QueueArray.h>
const int wls_lb_pin = 32;
const int wls_ub_pin = 34;
const int wls_gnd_pin = 36;

void wls_setup() {
	pinMode(wls_lb_pin, INPUT_PULLUP);
	digitalWrite(wls_lb_pin, HIGH);
	pinMode(wls_ub_pin, INPUT_PULLUP);
	digitalWrite(wls_ub_pin, HIGH);
}

void wls_control() {

}

// DHT11
#include <dht.h>
const int dht11_vcc_pin = 40;
const int dht11_out_pin = 42;
const int dht11_gnd_pin = 44;

void dht11_setup() {
	
}

// TDS
#include <OneWire.h>
#include <DallasTemperature.h>

// RTC
#include <Wire.h>
#include <RTClib.h>
RTC_DS1307 rtc;

void rtc_setup() {
	rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));
	rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

// LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal lcd(0x3F, 16, 2);

void lcd_setup() {
	lcd.begin();
	lcd.backlight();
}

/* ******** MAIN PROGRAM ******** */

void setup() {
	ledRelay_setup();
	pir_setup();
	wls_setup();
	dht11_setup();

}

void loop() {

}