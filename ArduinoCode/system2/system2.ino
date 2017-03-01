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

// RTC
#include <Wire.h>
#include <RTClib.h>
RTC_DS3231 rtc;
DateTime now;

unsigned long CurrentTime;

void rtc_setup() {
    rtc.begin();
    // rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // rtc.adjust(DateTime(2017, 3, 2, 0, 0, 0));
}

void Timer(unsigned long &LastActiveTime, bool &TimeLocked, int LockTime, int OperateTime) {
    DateTime now = rtc.now();
    CurrentTime = now.secondstime();
    if ((CurrentTime <= LastActiveTime + LockTime)) {
        TimeLocked = true;
    }
    else {
        TimeLocked = false;
        if (CurrentTime >= LastActiveTime + LockTime + OperateTime) {
            LastActiveTime = CurrentTime;
        }
    }
}

// DHT11
#include <dht.h>
dht dht11;

unsigned long dht11_LastActiveTime;
bool dht11_TimeLocked;
int dht11_LockTime = 2;
int dht11_OperateTime = 0;

int dht11_temp;
int dht11_humd;

void dht11_setup() {
    pinMode(dht11_vcc_pin, OUTPUT);
    digitalWrite(dht11_vcc_pin, HIGH);
    pinMode(dht11_gnd_pin, OUTPUT);
    digitalWrite(dht11_gnd_pin, LOW);
}

void dht11_control() {
    Timer(dht11_LastActiveTime, dht11_TimeLocked, dht11_LockTime, dht11_OperateTime);
    if (!dht11_TimeLocked) {
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
            Serial.println("Nhiet do dam bao!");
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
    }
}

// LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Convert time indices to two-char form
String TwoChar(int data_num) {
    String str = String(data_num);
    if (str.length() == 1) {
        str = "0" + str;
    }
    return str;
}

void lcd_setup() {
    lcd.begin();
    lcd.backlight();
}

unsigned long lcd_LastActiveTime;
bool lcd_TimeLocked;
int lcd_LockTime = 2;
int lcd_OperateTime = 0;

void lcd_control() {
    Timer(lcd_LastActiveTime, lcd_TimeLocked, lcd_LockTime, lcd_OperateTime);
    static int tracker = 0;
    if (!lcd_TimeLocked) {
        tracker++;
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
        lcd.print("KK  " + String(dht11_temp) + "C  " + String(dht11_humd) + "%");
    }
}


void setup() {
    Serial.begin(9600);
    dht11_setup();
    rtc_setup();
    lcd_setup();
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
    DateTime now = rtc.now();
    CurrentTime = now.secondstime();
    static bool init = true;
    if (init) {
        dht11_LastActiveTime = CurrentTime;
        lcd_LastActiveTime = CurrentTime;
        init = false;
    }
    dht11_control();
    dht11_temp = dht11.temperature;
    dht11_humd = dht11.humidity;
    lcd_control();
}