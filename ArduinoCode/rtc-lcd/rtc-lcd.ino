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

const int VCC1 = 43;
const int VCC2 = 45;
// const int VCC3 = 3;

void setup() {
    Serial.begin(9600);
    // Initialize LCD
    lcd.begin();
    lcd.backlight();

    // Check and set up rtc
    rtc.begin();
    if (! rtc.begin()) {
        Serial.println("Couldn't find RTC");
    }

    if (! rtc.isrunning()) {
        Serial.println("RTC is NOT running!");
    }
    
    // Adjust time
    // reset the clock
    // rtc.adjust(DateTime(2013, 1, 1, 0, 0, 0));
    // adjust the clock according to the computer
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // Set up VCCs

    pinMode(VCC1, OUTPUT);
    pinMode(VCC2, OUTPUT);
    // pinMode(VCC3, OUTPUT);
    digitalWrite(VCC1, HIGH);
    digitalWrite(VCC2, HIGH);
    // digitalWrite(VCC3, HIGH);
}

String TwoChar(int data_num) {
    String str = String(data_num);
    if (str.length() == 1) {
        str = "0" + str;
    }
    return str;
}

void loop() {
    Serial.println("BEGINNNNN");
    DateTime now = rtc.now();

    Serial.println(now.secondstime());
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
    Serial.println(day + "/" + month + "/" + year + " " + weekday[now.dayOfTheWeek()]);    
    lcd.setCursor(4, 1);
    lcd.print(hour + ":" + minute + ":" + second);

    int prev_time = now.secondstime();
    now = rtc.now();
    delay(1000);
}