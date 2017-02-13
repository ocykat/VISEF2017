/*
Arduino model: UNO
+ I2C devices:
	- RTC: 0x68
	- LCD I2C Address: 0x3F
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x3F, 16, 2);

const int VCC1 = 8;
const int VCC2 = 9;

void setup() {

	// Initialize LCD
	lcd.begin();
	lcd.backlight();

	// Check and set up rtc
	if (! rtc.begin()) {
		Serial.println("Couldn't find RTC");
	}

	if (! rtc.isrunning()) {
		Serial.println("RTC is NOT running!");
	}
	
	rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	// rtc.adjust(DateTime(2013, 1, 1, 0, 0, 0));
	// Set up VCCs

	pinMode(VCC1, OUTPUT);
	pinMode(VCC2, OUTPUT);
	digitalWrite(VCC1, HIGH);
	digitalWrite(VCC2, HIGH);
}

String TwoChar(int data_num) {
	String str = String(data_num);
	if (str.length() == 1) {
		str = "0" + str;
	}
	return str;
}

void loop() {

	DateTime now = rtc.now();
	String day, month, year, hour, minute, second;
	String weekday[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

	day = TwoChar(now.day());
	month = TwoChar(now.month());
	year = String(now.year() % 100);
	hour = TwoChar(now.hour());
	minute = TwoChar(now.minute());
	second = TwoChar(now.second());

	lcd.setCursor(2, 0);
	lcd.print(day + "/" + month + "/" + year + " " + weekday[now.dayOfTheWeek()]);
	
	lcd.setCursor(4, 1);
	lcd.print(hour + ":" + minute + ":" + second);

	int prev_time = now.secondstime();
	now = rtc.now();
	while (now.secondstime() - prev_time <= 1) {}
}