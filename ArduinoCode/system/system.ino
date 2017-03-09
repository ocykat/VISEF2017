/*
Thuc Hanh High school - HCMUE
A project on Household Aeroponic System
Arduino Model: Arduino Mega 2560
*/

/* ====== PIN LIST ====== */

// LED Relays
// LED Relays
const int relay_vcc_pin = A0; // analog pin
const int relay_led_pin = A1; // analog pin
const int relay_pump_pin = A2; // analog pin
// const int relay_gnd_pin = 7;

// PIR
const int pir_out_pin = 50;
const int pir_vcc_pin = 52;
const int pir_gnd_pin = 53;

// DHT11
const int dht11_out_pin = 42;
const int dht11_vcc_pin = 44;
const int dht11_gnd_pin = 45;

// WLS
const int wls_lb_pin = 38;
const int wls_ub_pin = 36;
const int wls_gnd1_pin = 39;
const int wls_gnd2_pin = 1;

// TDS
// tds_temp pins
const int tds_onewire_bus = 36;
const int tds_temp_vcc_pin = 34;
const int tds_temp_gnd_pin = 35;
// tds_EC pins
const int tds_EC_Aout_pin = A15; // analog OUT
const int tds_EC_gnd_pin = 32;
const int tds_EC_vcc_pin = 33;

/* ========== COMPONENT SPECIFICATION ========== */

/* __________RTC__________ */
// Libraries
#include <Wire.h>
#include <RTClib.h>

// Object
RTC_DS3231 rtc;

// Time recorder
unsigned long CurrentTime;

// Start/end of day - Day/night mode
int DayStart = 6;
int DayEnd = 18;
bool DayMode;

void rtc_setup() {
    rtc.begin();
    rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void Timer(unsigned long &LastActive, bool &Locked, int LockTime, int OperateTime) {
    if ((CurrentTime < LastActive + LockTime)) {
        Locked = true;
    }
    else {
        Locked = false;
        if (CurrentTime >= LastActive + LockTime + OperateTime) {
            LastActive = CurrentTime;
        }
    }
}

void DayModeCheck(int hour) {
    if ((hour >= DayStart) && (hour < DayEnd)) {
        DayMode = true;
    }
    else {
        DayMode = false;
    }
}

/* __________PIR__________ */
// Value
int pir_val;

void pir_setup() {
    pinMode(pir_vcc_pin, OUTPUT);
    digitalWrite(pir_vcc_pin, HIGH);

    pinMode(pir_gnd_pin, OUTPUT);
    digitalWrite(pir_gnd_pin, LOW);
}

void pir_read() {
    pir_val = digitalRead(pir_out_pin);
}

/* __________Relay__________ */
// Time management
unsigned long relay_pump_LastActive;
bool relay_pump_Locked;
const int relay_pump_LockTime = 300;
const int relay_pump_OperateTime = 5;

void relay_setup() {
    pinMode(relay_vcc_pin, OUTPUT);
    digitalWrite(relay_vcc_pin, HIGH);

    // pinMode(relay_gnd_pin, OUTPUT);
    // digitalWrite(relay_gnd_pin, LOW);

    pinMode(relay_led_pin, OUTPUT);

    pinMode(relay_pump_pin, OUTPUT);
}

void relay_led_control() {
    if (DayMode) {
        digitalWrite(relay_led_pin, LOW);
    }
    else {
        if (pir_val == HIGH) {
            digitalWrite(relay_led_pin, LOW);
        }
        else {
            digitalWrite(relay_led_pin, HIGH);
        }
    }
}

void relay_pump_control() {
    if (!relay_pump_Locked) {
        digitalWrite(relay_pump_pin, LOW);
    }
    else {
        digitalWrite(relay_pump_pin, HIGH);
    }
}

/* __________DHT11__________ */
// Library
#include <dht.h>

// Object
dht dht11;

// Values
int dht11_temp;
int dht11_humd;

// Time management
unsigned long dht11_LastActive;
bool dht11_Locked;
const int dht11_LockTime = 2;
const int dht11_OperateTime = 0;

void dht11_setup() {
    pinMode(dht11_vcc_pin, OUTPUT);
    digitalWrite(dht11_vcc_pin, HIGH);

    pinMode(dht11_gnd_pin, OUTPUT);
    digitalWrite(dht11_gnd_pin, LOW);
}

void dht11_read() {
    if (!dht11_Locked) {
        dht11.read11(dht11_out_pin);
        dht11_temp = dht11.temperature;
        dht11_humd = dht11.humidity;
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

// Objects
OneWire tds_onewire(tds_onewire_bus);
DallasTemperature tds_temp_sensor(&tds_onewire);

// Time management
unsigned long tds_LastActive;
bool tds_Locked;
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

void tds_read() {
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

/* __________WLS__________ */
#include <QueueArray.h>

// Waterlevel
int wls_waterlevel;

void wls_setup() {
    pinMode(wls_lb_pin, INPUT_PULLUP);
    pinMode(wls_ub_pin, INPUT_PULLUP);

    pinMode(wls_gnd1_pin, OUTPUT);
    digitalWrite(wls_gnd1_pin, LOW);
    pinMode(wls_gnd2_pin, OUTPUT);
    digitalWrite(wls_gnd2_pin, LOW);
}

void wls_read() {
    if (tds_Locked) {
        // Pull pins to HIGH
        digitalWrite(wls_lb_pin, HIGH);
        digitalWrite(wls_ub_pin, HIGH);

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
    else {
        digitalWrite(wls_lb_pin, LOW);
        digitalWrite(wls_ub_pin, LOW);
    }
}

/* __________LCD__________ */
// Libraries
#include <LiquidCrystal_I2C.h>

// Object
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Time management
unsigned long lcd_LastActive;
bool lcd_Locked;
const int lcd_LockTime = 1;
const int lcd_OperateTime = 0;

// Tracker - determine the 2nd line
int lcd_tracker = 0;

void lcd_setup() {
    lcd.begin();
    lcd.backlight();
}

String lcd_twochar(int num) {
    String str = String(num);
    if (str.length() == 1) {
        str = "0" + str;
    }
    return str;
}

void lcd_show_time(int day, int month, int year, int hour, int minute, int second, int weekday) {
    if (!lcd_Locked) {
        lcd.setCursor(0, 0);
        String WeekDay[7] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA"};
        String WeekDay_vi[7] = {"CN", "T2", "T3", "T4", "T5", "T6", "T7"};

        String DD = lcd_twochar(day);
        String MM = lcd_twochar(month);
        String YY = lcd_twochar(year % 100);
        String hh = lcd_twochar(hour);
        String mm = lcd_twochar(minute);
        String ss = lcd_twochar(second);

        lcd.setCursor(1, 0);
        lcd.print(hh + ":" + mm + " " + WeekDay_vi[weekday] + " " + DD + "/" + MM);
    }
}

void lcd_clearline(int line) {
    lcd.setCursor(0, line);
    for (int i = 0; i < 16; i++) {
        lcd.print(" ");
    }
}

void lcd_show_dht11() {
    lcd_clearline(1);
    lcd.setCursor(0, 1);
    lcd.print("KK");
    lcd.setCursor(5, 1);
    lcd.print(String(dht11_temp) + "C");
    lcd.setCursor(11, 1);
    lcd.print(String(dht11_humd) + "%");
}

void lcd_show_wls() {
    lcd_clearline(1);
    lcd.setCursor(0, 1);
    lcd.print("MucDD");
    lcd.setCursor(8, 1);
    switch (wls_waterlevel) {
        case -1: {
            lcd.print("Thieu!");            
        }
            break;
        case 0: {
            lcd.print("DuDung");
        }
            break;
        case 1: {
            lcd.print("QuaDay!");
        }
            break;
        case 2: {
            lcd.print("DangDoc..");
        }
            break;
    }
}

void lcd_show_tds() {
    lcd_clearline(1);
    lcd.setCursor(0, 1);
    lcd.print("DD");
    lcd.setCursor(4, 1);
    lcd.print(String(tds_temp_temperature) + "C");
    lcd.setCursor(9, 1);
    lcd.print(String(tds_EC_ppm) + "ppm");
}

void lcd_show_sensors() {
    if ((!lcd_Locked) && (lcd_tracker < 9)) {
        lcd_tracker++;
        if (lcd_tracker <= 3) {
            lcd_show_dht11();
        }
        else if (lcd_tracker <= 6) {
            lcd_show_wls();
        }
        else {
            lcd_show_tds();
        }
    }
    if (!DayMode) {
        if (lcd_tracker == 9) {
            lcd_tracker = 0;
        }
    }
}

void lcd_show_ledActiveTime(int hour, int minute) {
    if ((!lcd_Locked) && (DayMode) && (lcd_tracker >= 9)) {
        lcd_tracker++;
        lcd_clearline(1);
        lcd.setCursor(0, 1);
        lcd.print("DenSang");
        lcd.setCursor(9, 1);
        lcd.print(lcd_twochar(hour - DayStart) + ":" + lcd_twochar(minute));
        if (lcd_tracker == 12) {
            lcd_tracker = 0;
        }
    }
}

/* __________Serial Monitor__________ */
// Time management
unsigned long sm_LastActive;
bool sm_Locked;
const int sm_LockTime = 1;
const int sm_OperateTime = 0;

void sm_rtc(int day, int month, int year, int hour, int minute, int second) {
    if (!sm_Locked) {
        Serial.print("+ RTC: ");
        Serial.print(String(day) + "/" + String(month) + "/" + String(year) + " - ");
        Serial.println(String(hour) + ":" + String(minute) + ":" + String(second));     
    }
}

void sm_dht11() {
    if (!sm_Locked) {
        Serial.print("+ DHT11: ");
        Serial.println(String(dht11_temp) + "C - " + String(dht11_humd) + "%");
    }

}

void sm_wls() {
    if (!sm_Locked) {
        Serial.print("+ WLS: ");
        switch (wls_waterlevel) {
            case -1: {
                Serial.println("Below LB");         
            }
                break;
            case 0: {
                Serial.println("Between LB and UB");
            }
                break;
            case 1: {
                Serial.print("Above UB");
            }
                break;
            case 2: {
                Serial.print("Reading...");
            }
                break;
        }
    }
}

void sm_tds() {
    if (!sm_Locked) {
        Serial.print("+ TDS: ");
        Serial.println(String(tds_temp_temperature) + "C - " + String(tds_EC_ppm) + "ppm");
        Serial.println("LCD tracker: " + String(lcd_tracker));
    }
}

/* ========== MAIN PROGRAM ==========*/
void setup() {
    Serial.begin(9600);
    rtc_setup();    
    pir_setup();
    relay_setup();
    dht11_setup();
    wls_setup();
    tds_setup();
    lcd_setup();
}

void loop() {
    DateTime now = rtc.now();
    CurrentTime = now.secondstime();
    static bool Initialized = false;

    if (!Initialized) {
        // Set LastActive Time
        dht11_LastActive = CurrentTime;
        tds_LastActive = CurrentTime;
        lcd_LastActive = CurrentTime;
        relay_pump_LastActive = CurrentTime;
        sm_LastActive = CurrentTime;

        // Animation
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Starting");
        for (int i = 0; i < 3; i++) {
            lcd.print(".");
            delay(500);
        }
        lcd.clear();

        Initialized = true;     
    }
    else {
        // Time management
        DayModeCheck(now.hour());
        Timer(dht11_LastActive, dht11_Locked, dht11_LockTime, dht11_OperateTime);
        Timer(tds_LastActive, tds_Locked, tds_LockTime, tds_OperateTime);
        Timer(lcd_LastActive, lcd_Locked, lcd_LockTime, lcd_OperateTime);
        Timer(relay_pump_LastActive, relay_pump_Locked, relay_pump_LockTime, relay_pump_OperateTime);
        Timer(sm_LastActive, sm_Locked, sm_LockTime, sm_OperateTime);

        // Sensor reads
        pir_read();
        dht11_read();
        wls_read();
        tds_read();

        // Relay control
        relay_led_control();
        relay_pump_control();

        // Show data on display
        lcd_show_time(now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second(), now.dayOfTheWeek());
        lcd_show_sensors();
        lcd_show_ledActiveTime(now.hour(), now.minute());

        // Serial monitor
        sm_rtc(now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());
        sm_dht11();
        sm_wls();
        sm_tds();
    }
}