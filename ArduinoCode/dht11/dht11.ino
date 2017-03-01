#include <RTClib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
/*
Arduino model: UNO
*/

// DHT11 library
#include <dht.h>

// DHT object
dht dht11;
RTC_DS3231 rtc;

LiquidCrystal_I2C lcd(0x3F, 16, 2);

// DHT11 out pin
// DHT11
const int dht11_vcc_pin = 22;
const int dht11_out_pin = 24;
const int dht11_gnd_pin = 26;

unsigned long CurrentTime;
unsigned long dht11_LastActiveTime;
bool dht11_TimeLocked;
int dht11_LockTime = 2;
int dht11_OperateTime = 0;

unsigned long lcd_LastActiveTime;
bool lcd_TimeLocked;
int lcd_LockTime = 2;
int lcd_OperateTime = 0;

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

void setup() {
    Serial.begin(9600);
    pinMode(dht11_vcc_pin, OUTPUT);
    digitalWrite(dht11_vcc_pin, HIGH);
    pinMode(dht11_gnd_pin, OUTPUT);
    digitalWrite(dht11_gnd_pin, LOW);
    lcd.begin();
    lcd.backlight();
    rtc.begin();
    rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void dht11_control() {
    // Printing out temperature on Serial monitor
    static bool init = true;
    DateTime now = rtc.now();
    CurrentTime = now.secondstime();

    if (init) {
        dht11_LastActiveTime = CurrentTime;
        lcd_LastActiveTime = CurrentTime;
        init = false;
    }

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

    // Wait for 2 seconds before reading new signal
    // delay(1000);
}

void loop() {
    DateTime now = rtc.now();
    CurrentTime = now.secondstime();
    dht11_control();
    int temp = dht11.temperature;
    int humd = dht11.humidity;
    Timer(lcd_LastActiveTime, lcd_TimeLocked, lcd_LockTime, lcd_OperateTime);
    if (!lcd_TimeLocked) {
        lcd.clear();
        lcd.print("KK " + String(temp) + "C  " + String(humd) + "%");
    }
}