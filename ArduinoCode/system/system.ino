/*
Thuc Hanh High school - HCMUE
A project on Household Aeroponic System
Arduino Model: Arduino Mega 2560
*/

/* ====== PIN LIST ====== */

// LED Relays
const int relay_vcc_pin = 4;
const int relay_led_pin = 5;
const int relay_pump_pin = 6;
const int relay_gnd_pin = 7;

// PIR
const int pir_vcc_pin = 28;
const int pir_out_pin = 30;
const int pir_gnd_pin = 32;

// WLS
const int wls_lb_pin = 36;
const int wls_ub_pin = 38;
const int wls_gnd1_pin = 40;
const int wls_gnd2_pin = 42;

// DHT11
const int dht11_vcc_pin = 22;
const int dht11_out_pin = 24;
const int dht11_gnd_pin = 26;

// TDS
// tds_EC pins
const int tds_EC_Aout_pin = A15; // analog OUT
const int tds_EC_gnd_pin = 51;
const int tds_EC_vcc_pin = 53;
// tds_temp pins
const int tds_onewire_bus = 48;
const int tds_temp_vcc_pin = 49;
const int tds_temp_gnd_pin = 50;

/* ========== COMPONENT SPECIFICATION ========== */

/* __________RTC__________ */
#include <Wire.h>
#include <RTClib.h>
RTC_DS3231 rtc;
DateTime now;

// Set up RTC
void rtc_setup() {
	rtc.begin();
	rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));
	rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

// Day/Night mode
bool DayMode() {
	if ((now.hour() >= 6) && (now.hour() <= 17)) {
		return true;
	}
	else {
		return false;
	}
}

// Current time recorder
unsigned long CurrentTime;

// Serial
unsigned long Serial_LastActive;
bool Serial_Locked = true;
const int Serial_LockTime = 1;
const int Serial_OperateTime = 0;

void Timer(unsigned long &LastActive, bool &Locked, int LockTime, int OperateTime) {
	if ((CurrentTime <= LastActive + LockTime)) {
		Locked = true;
	}
	else {
		Locked = false;
		if ((CurrentTime >= LastActive + LockTime + OperateTime) || (OperateTime == 0)) {
			LastActive = CurrentTime;
		}
	}
}

/* __________Relay__________ */
// Pump
unsigned long relay_pump_LastActive;
bool relay_pump_Locked = true;
const int relay_pump_LockTime = 300;
const int relay_pump_OperateTime = 5;

void relay_setup() {
	pinMode(relay_vcc_pin, OUTPUT);
	digitalWrite(relay_vcc_pin, HIGH);

	pinMode(relay_gnd_pin, OUTPUT);
	digitalWrite(relay_gnd_pin, LOW);

	pinMode(relay_led_pin, OUTPUT);
	pinMode(relay_pump_pin, OUTPUT);
}

void relay_led_control() {
	if (DayMode()) {
		digitalWrite(relay_led_pin, HIGH);
	}
	else {
		if (pir_read() == HIGH) {
			digitalWrite(relay_led_pin, LOW);
		}
		else {
			digitalWrite(relay_led_pin, HIGH);
		}
	}
}

void relay_pump_control() {
	if (!relay_pump_Locked) {
		digitalWrite(relay_pump_pin, HIGH);
	}
}

/* __________PIR__________ */
void pir_setup() {
	pinMode(pir_out_pin, INPUT);
}

int pir_read() {
	return digitalRead(pir_out_pin);
}

/* __________WLS__________ */
#include <QueueArray.h>

int wls_waterlevel;

void wls_setup() {
	pinMode(wls_lb_pin, INPUT_PULLUP);
	digitalWrite(wls_lb_pin, HIGH);
	pinMode(wls_ub_pin, INPUT_PULLUP);
	digitalWrite(wls_ub_pin, HIGH);

	pinMode(wls_gnd1_pin, OUTPUT);
	digitalWrite(wls_gnd1_pin, LOW);
	pinMode(wls_gnd2_pin, OUTPUT);
	digitalWrite(wls_gnd2_pin, LOW);
}

void wls_read(int &wls_waterlevel) {
	// Digital inputs are converted to signals of 0 and 1
	// 0 ~ circuit is opened; 1 ~ circuit is closed
	int wls_lb_read;
	int wls_ub_read;
	
	// A queue for each sensor
	static QueueArray <int> wls_lb_sigqueue;
	static QueueArray <int> wls_ub_sigqueue;

	// Set variables to count LOW signals from the input pins
	static int wls_lb_lsigcnt = 0;
	static int wls_ub_lsigcnt = 0;

	// Read sensor 
	int wls_lb_val = digitalRead(wls_lb_pin);
	int wls_ub_val = digitalRead(wls_ub_pin);

	// Append signals to the queue
	wls_lb_sigqueue.enqueue(wls_lb_val);
	wls_ub_sigqueue.enqueue(wls_ub_val);
	
	// Count LOW signals from input pins
	if (wls_lb_val == LOW) {
		wls_lb_lsigcnt++;
	}
	if (wls_ub_val == LOW) {
		wls_ub_lsigcnt++;
	}

	// For the filter to start, the first 100 signals must be appended to each queue
	if (wls_lb_sigqueue.count() == 100) {   
	
		// Set accuracy rate at >30/100 signals
		if (wls_lb_lsigcnt <= 30) {
			wls_lb_read = 0;
		}
		else {
			wls_lb_read = 1;
		}

		if (wls_ub_lsigcnt <= 30) {
			wls_ub_read = 0;
		}
		else {
			wls_ub_read = 1;
		}

		// Remove the first element of each queue
		if (wls_lb_sigqueue.front() == LOW) {
			wls_lb_lsigcnt--;
		}

		if (wls_ub_sigqueue.front() == LOW) {
			wls_ub_lsigcnt--;
		}

		wls_lb_sigqueue.dequeue();
		wls_ub_sigqueue.dequeue();

		// Return water level
		wls_waterlevel = -1 + wls_lb_read + wls_ub_read;
	}
	else {
		wls_waterlevel = 2;
	}
}

/* __________DHT11__________ */
#include <dht.h>

// Time management
unsigned long dht11_LastActive;
bool dht11_Locked = true;
const int dht11_LockTime = 2;
const int dht11_OperateTime = 0;

dht dht11; //dht object

int dht11_temp;
int dht11_humd;

void dht11_read(int &dht11_temp, int &dht11_humd) {
	if (!dht11_Locked) {
		int check = dht11.read(dht11_out_pin);
		delay(2000);
		check = dht11.read(dht11_out_pin);

		switch (check) {
			case DHTLIB_OK:  
				Serial.print("OK,\t"); 
				break;
			case DHTLIB_ERROR_CHECKSUM: 
				Serial.print("Checksum error,\t"); 
				break;
			case DHTLIB_ERROR_TIMEOUT: 
				Serial.print("Time out error,\t"); 
				break;
			default: 
				Serial.print("Unknown error,\t"); 
				break;
		}
		dht11_temp = (int) dht11.temperature;
		dht11_humd = (int) dht11.humidity;
		// ERROR CHECK
		if ((dht11_temp > 50) || (dht11_temp < 0)) {
			Serial.println("DHT11_ERROR!!!");
		}
	}
}

/* __________TDS__________ */
#include <OneWire.h>
#include <DallasTemperature.h>

// Resistors
const int tds_EC_R_chosen = 1000; // any resistor with the resistance of >300 ohm would work
const int tds_EC_R_pin = 25; // resistance of the pin

// Calibration constants
const float tds_EC_PPM_conversion = 0.7; // EC to PPM conversion constant
const float tds_EC_tempcoef = 0.019; // Temperature coefficient
const float tds_EC_K = 2.88;

// Initialize libraries' objects
OneWire tds_onewire(tds_onewire_bus);
DallasTemperature tds_temp_sensor(&tds_onewire);

// Time management
unsigned long tds_LastActive;
bool tds_Locked = true;
const int tds_LockTime = 10;
const int tds_OperateTime = 0;

// Values
int tds_temp_temperature;
int tds_EC_ppm;

void tds_setup() {
	// Set up TDS Temperature sensor pins
	pinMode(tds_temp_vcc_pin, OUTPUT);
	digitalWrite(tds_temp_vcc_pin, HIGH);

	pinMode(tds_temp_gnd_pin, OUTPUT);
	digitalWrite(tds_temp_gnd_pin, LOW);

	// Set up TDS EC sensor pins
	pinMode(tds_EC_vcc_pin, OUTPUT);

	pinMode(tds_EC_gnd_pin, OUTPUT);
	digitalWrite(tds_EC_gnd_pin, LOW);

	pinMode(tds_EC_Aout_pin, INPUT);

	tds_temp_sensor.begin();
}

void tds_read(int &tds_temp_temperature, int &tds_EC_ppm) {
	if (!tds_Locked) {
		// _*Temperature sensor*_
		// Read solution's temperature
		tds_temp_sensor.requestTemperatures();
		tds_temp_temperature = tds_temp_sensor.getTempCByIndex(0);
		
		// _*EC sensor*_
		// Provide tds_EC_vcc_pin with HIGH voltage to start measuring
		// Then stop after measurement has been done
		digitalWrite(tds_EC_vcc_pin, HIGH);
		// Read EC's analog input twice, since first read returns incorrect result
		float tds_EC_V_raw;
		for (int i = 0; i < 2; i++) {
			tds_EC_V_raw = analogRead(tds_EC_Aout_pin);
		}
		digitalWrite(tds_EC_vcc_pin, LOW);

		// Calculations
		float tds_EC_V_in = 5;
		float tds_EC_V_drop = (tds_EC_V_in * tds_EC_V_raw) / 1024.0;
		// Solution's resistance
		float tds_EC_R_solution = (tds_EC_V_drop * tds_EC_R_chosen) / (tds_EC_V_in - tds_EC_V_drop) - tds_EC_R_pin;
		// Solution's electrical conductivity
		float tds_EC_ECval = 1000 / (tds_EC_R_solution * tds_EC_K);
		// Solution's electrical conductivity at 25 degree Celcius
		float tds_EC_EC25val = tds_EC_ECval / (1 + tds_EC_tempcoef * (tds_temp_temperature - 25.0));
		// Solution's ppm
		tds_EC_ppm = (tds_EC_EC25val) * (tds_EC_PPM_conversion * 1000);
	}
}

/* __________LCD__________ */
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Time management
unsigned long lcd_LastActive;
bool lcd_Locked = true;
const int lcd_LockTime = 1;
const int lcd_OperateTime = 0;

// Set up
void lcd_setup() {
	lcd.begin();
	lcd.backlight();
}

// Convert time indices to two-char form
String TwoChar(int data_num) {
	String str = String(data_num);
	if (str.length() == 1) {
		str = "0" + str;
	}
	return str;
}

// Display
void lcd_control() {
	static int lcd_TimeTracker = 0;
	
	if (!lcd_Locked) {
		lcd_TimeTracker++;
		String day, month, year, hour, minute, second;
		String weekday[7] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA"};

		day = TwoChar(now.day());
		month = TwoChar(now.month());
		year = String(now.year() % 100);
		hour = TwoChar(now.hour());
		minute = TwoChar(now.minute());
		second = TwoChar(now.second());

		// Always print out time
		lcd.setCursor(1, 0);
		lcd.print(hour + ":" + minute + " " + weekday[now.dayOfTheWeek()] + " " + day + "/" + month);
		lcd.setCursor(0, 1);
		lcd.print("                ");
		lcd.setCursor(0, 1);
		// Print out other info every 3 secs
		if (lcd_TimeTracker <= 3) {
			lcd.print("KK " + String(dht11_temp) + "C  " + String(dht11_humd) + "%");
			if ((dht11_temp < 19) || (dht11_temp > 24)) {
				lcd.setCursor(9, 1);
				lcd.print("!");
			}
			if (dht11_humd <= 40) {
				lcd.setCursor(14, 1);
				lcd.print("!");
			}
		}
		else if (lcd_TimeTracker <= 6) {
			lcd.print("DD  " + String(tds_temp_temperature) + "C  " + String(tds_EC_ppm) + "ppm");
			if ((tds_EC_ppm < 500) || (tds_EC_ppm > 800)) {
				lcd.setCursor(15, 1);
				lcd.print("!");
			}
		}
		else if (lcd_TimeTracker <= 9) {
			lcd.print("MUCDD  ");
			switch (wls_waterlevel) {
				case -1:
					lcd.print("Thieu!");
					break;
				case 0:
					lcd.print("DuDung");
					break;
				case 1:
					lcd.print("QuaDay!");
					break;
				case 2:
					lcd.print("...");
			}
			if (lcd_TimeTracker == 9) lcd_TimeTracker = 0;
		}
	}
}

void Serial_Monitor() {
	if (!Serial_Locked) {
		// Read PIR
		if (pir_read() == HIGH) {
			Serial.println("+ PIR: Detecting motion!");
		}
		else {
			Serial.println("+ PIR: Motion not detected");
		}

		// Read WLS
		switch (wls_waterlevel) {
			case -1:
				Serial.println("+ WLS: Below LB!");
				break;
			case 0:
				Serial.println("+ WLS: Between LB and UB!");
				break;
			case 1:
				Serial.println("+ WLS: Above UB!");
				break;
			case 2:
				Serial.println("+ WLS: Initializing...");
		}

		// Read DHT11
		Serial.print("+ DHT11: ");
		Serial.print("Temp: " + String(dht11_temp) + "C; ");
		Serial.println("Humd: " + String(dht11_humd) + "%");

		// Read TDS
		Serial.print("+ TDS: ");
		Serial.print("Temp: " + String(tds_temp_temperature) + "C; ");
		Serial.println("Conc: " + String(tds_EC_ppm) + "ppm");
		Serial.println("**********************************************");
	}
}

/* ========== MAIN PROGRAM ========== */

void setup() {
	Serial.begin(9600);
	relay_setup();
	pir_setup();
	wls_setup();
	tds_setup();
	rtc_setup();
	lcd_setup();
}

void loop() {
	now = rtc.now();
	CurrentTime = now.secondstime();
	static int startup = 0;

	if (startup == 0) {
		relay_pump_LastActive = CurrentTime;
		dht11_LastActive = CurrentTime;
		tds_LastActive = CurrentTime;
		lcd_LastActive = CurrentTime;
		Serial_LastActive = CurrentTime;
		Serial.print("Initializing...");
		lcd.setCursor(2, 1);
		for (int i = 0; i < 5; i++) {
			Serial.print(".");
			lcd.print(".");
			delay(500);
		}
		Serial.println();
		// dht11_read(dht11_temp, dht11_humd);
		lcd.clear();
		if (wls_waterlevel != 2) {
			startup = 1;
		}
	}
	else {
		Timer(relay_pump_LastActive, relay_pump_Locked, relay_pump_LockTime, relay_pump_OperateTime);
		Timer(dht11_LastActive, dht11_Locked, dht11_LockTime, dht11_OperateTime);
		Timer(tds_LastActive, tds_Locked, tds_LockTime, tds_OperateTime);     
		Timer(lcd_LastActive, lcd_Locked, lcd_LockTime, lcd_OperateTime);
		Timer(Serial_LastActive, Serial_Locked, Serial_LockTime, Serial_OperateTime);
		relay_led_control();
		relay_pump_control();
		dht11_read(dht11_temp, dht11_humd);
		wls_read(wls_waterlevel);
		tds_read(tds_temp_temperature, tds_EC_ppm);
		lcd_control();
		Serial_Monitor();
	}
}