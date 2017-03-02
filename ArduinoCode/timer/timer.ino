/*
Arduino Model: UNO
*/

const int led_pin = 13;
unsigned long led_LastActiveTime;
bool led_TimeLocked;
const int led_LockTime = 500;
const int led_OperateTime = 1000;
unsigned long CurrentTime;

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
        Timer(led_LastActiveTime, led_TimeLocked, led_LockTime, led_OperateTime);
        if (!led_TimeLocked) {
            digitalWrite(led_pin, HIGH);
        }
        else {
            digitalWrite(led_pin, LOW);
        }
    }
}