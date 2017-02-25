/*
Arduino Model: UNO
*/

const int led_pin = 13;
unsigned long led_LastActiveTime;
bool led_TimeLocked;
const int led_LockTime = 500;
const int led_DelayTime = 1000;
unsigned long CurrentTime;

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

void setup() {
    pinMode(led_pin, OUTPUT);
}

void loop() {
    CurrentTime = millis();
    static bool firsttime = true;
    if (firsttime) {
        led_LastActiveTime = CurrentTime;
        firsttime = false;
    }
    else {
        Timer(led_LastActiveTime, led_TimeLocked, led_LockTime, led_DelayTime);
        if (!led_TimeLocked) {
            digitalWrite(led_pin, HIGH);
        }
        else {
            digitalWrite(led_pin, LOW);
        }
    }
}