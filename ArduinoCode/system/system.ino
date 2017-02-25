/* ====== PIN LIST ====== */

// LED Relays
const int relay_vcc_pin = 4;
const int relay_led_pin = 5;
const int relay_pump_pin = 6;
const int relay_gnd_pin = 7;

// PIR
const int pir_vcc_pin = 24;
const int pir_out_pin = 26;
const int pir_gnd_pin = 28;

// WLS
const int wls_lb_pin = 32;
const int wls_ub_pin = 34;
const int wls_gnd1_pin = 36;
const int wls_gnd2_pin = 38;

// DHT11
const int dht11_vcc_pin = 40;
const int dht11_out_pin = 42;
const int dht11_gnd_pin = 44;

// TDS
// tds_EC pins
const int tds_EC_Aout_pin = A3; // analog OUT
const int tds_EC_gnd_pin = 51;
const int tds_EC_vcc_pin = 53;
// tds_temp pins
const int tds_onewire_bus = 48;
const int tds_temp_vcc_pin = 49;
const int tds_temp_gnd_pin = 50;

/* ======== Time Management ======== */
int CurrentTime;

// Pump
unsigned long relay_pump_LastActiveTime;
bool relay_pump_TimeLocked = true;
const int relay_pump_LockTime = 300;
const int relay_pump_DelayTime = 5;

// DHT11
unsigned long dht11_LastActiveTime;
bool dht11_TimeLocked = true;
const int dht11_LockTime = 2;
const int dht11_DelayTime = 0;

// TDS
unsigned long tds_LastActiveTime;
bool tds_TimeLocked = true;
const int tds_LockTime = 10;
const int tds_DelayTime = 0;

// LCD
unsigned long lcd_LastActiveTime;
bool lcd_TimeLocked = true;
const int lcd_LockTime = 1;
const int lcd_DelayTime = 0;

void Timer(unsigned long &LastActiveTime, bool &TimeLocked, int LockTime, int DelayTime) {
	if ((CurrentTime > LastActiveTime) && (CurrentTime <= LastActiveTime + LockTime)) {
		TimeLocked = true;
	}
	else {
		TimeLocked = false;
		if (CurrentTime >= LastActiveTime + LockTime + DelayTime) {
			LastActiveTime = CurrentTime;
		}
	}
}

/* ========== FUNCTIONS ========== */

/* __________Relay__________ */
void relay_setup() {
	pinMode(relay_vcc_pin, OUTPUT);
	digitalWrite(relay_vcc_pin, HIGH);

	pinMode(relay_gnd_pin, OUTPUT);
	digitalWrite(relay_gnd_pin, LOW);

	pinMode(relay_led_pin, OUTPUT);
	pinMode(relay_pump_pin, OUTPUT);
}

void relay_led_control() {
	if (DayMode) {
		digitalWrite(relay_led_pin, HIGH);
	}
	else {
		if (pir_read == HIGH) {
			digitalWrite(relay_led_pin, LOW);
		}
		else {
			digitalWrite(relay_led_pin, HIGH);
		}
	}
}

void relay_pump_control() {
	if (!relay_pump_TimeLocked) {
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

int wls_read() {
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
		static int wls_waterlevel = -1 + wls_lb_read + wls_ub_read;
		return wls_waterlevel;
	}
	else {
		// water level is set at 0 when the sensor is initializing
		return 0;
	}
}

/* __________DHT11__________ */
#include <dht.h>

dht dht11; //dht object

void dht11_read() {
	if (!dht11_TimeLocked) {
		dht11.read11(dht11_out_pin);
	}
}

int dht11_temp_result() {
	return (int) dht11.temperature;
}

int dht11_humd_result() {
	return (int) dht11.humidity;
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

// Solution's ppm
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

int tds_temp_read() {
	// Read solution's temperature
	tds_temp_sensor.requestTemperatures();
	float tds_temp_temperature = tds_temp_sensor.getTempCByIndex(0);
	return (int) tds_temp_temperature;
}

int tds_EC_read() {
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
	float tds_EC_EC25val = tds_EC_ECval / (1 + tds_EC_tempcoef * (tds_temp_read() - 25.0));
	// Solution's ppm
	float tds_EC_ppm = (tds_EC_EC25val) * (tds_EC_PPM_conversion * 1000);

	return (int) tds_EC_ppm;
}

/* __________RTC__________ */
#include <Wire.h>
#include <RTClib.h>
RTC_DS1307 rtc;
DateTime now = rtc.now();

// Set up RTC
void rtc_setup() {
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

/* __________LCD__________ */
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

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

// Showing time
void lcd_control() {
	DateTime now = rtc.now();
	static int lcd_TimeTracker = 0;
	if (!lcd_TimeLocked) {
		lcd_TimeTracker++;
		String day, month, year, hour, minute, second;
		String weekday[7] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA"};

		day = TwoChar(now.day());
		month = TwoChar(now.month());
		year = String(now.year() % 100);
		hour = TwoChar(now.hour());
		minute = TwoChar(now.minute());
		second = TwoChar(now.second());

		lcd.clear();
		lcd.setCursor(2, 0);
		lcd.print(hour + ":" + minute + " " + weekday[now.dayOfTheWeek()] + day + "/" + month);
		if (lcd_TimeTracker <= 3) {
			lcd.setCursor(3, 1);
			lcd.print("KK  " + String(dht11_temp_result()) + "C  " + String(dht11_humd_result()) + "%");
			if ((dht11_temp_result < 19) || (dht11_temp_result > 24)) {
				lcd.setCursor(10, 1);
				lcd.print("!");
			}
			if ((dht11_humd_result) <= 40) {
				lcd.setCursor(15, 1);
				lcd.print("!");
			}
		}
		else if (lcd_TimeTracker <= 6) {
			lcd.setCursor(1, 1);
			lcd.print("DD  " + String(tds_temp_read()) + "C  " + String(tds_EC_read()) + "ppm");
			if ((tds_EC_read < 500) || (tds_EC_read > 800)) {
				lcd.setCursor(16, 1);
				lcd.print("!");
			}
		}
		else {
			lcd.setCursor(2, 1);
			lcd.print("MUCDD  ");
			switch (wls_read()) {
				case -1:
					lcd.print("Thieu!");
					break;
				case 0:
					lcd.print("DuDung");
					break;
				case 1:
					lcd.print("QuaDay");
					break;
			}
			if (lcd_TimeTracker == 9) {
				lcd_TimeTracker = 0;
			}
		}
	}
}



/* ========== MAIN PROGRAM ========== */

void setup() {
	relay_setup();
	pir_setup();
	wls_setup();
	tds_setup();
	rtc_setup();
	lcd_setup();
}

void loop() {
	DateTime now = rtc.now();
	unsigned long CurrentTime = now.secondstime();
	static bool FirstTime = true;

	if (FirstTime) {
		relay_pump_LastActiveTime = CurrentTime;
		dht11_LastActiveTime = CurrentTime - 295;
		tds_LastActiveTime = CurrentTime;
		lcd_LastActiveTime = CurrentTime;
		delay(3000);
		FirstTime = false;
	}
	else {
		Timer(relay_pump_LastActiveTime, relay_pump_TimeLocked, relay_pump_LockTime, relay_pump_DelayTime);
		Timer(dht11_LastActiveTime, dht11_TimeLocked, dht11_LockTime, dht11_DelayTime);
		Timer(tds_LastActiveTime, tds_TimeLocked, tds_LockTime, tds_DelayTime);		
		Timer(lcd_LastActiveTime, lcd_TimeLocked, lcd_LockTime, lcd_DelayTime);
		relay_led_control();
		relay_pump_control();
		wls_read();
		dht11_read();
		lcd_control();
	}
}