/*
Arduino Model: UNO
*/

#include <QueueArray.h>

// WLS input pins
const int wls_lb_pin = 4;
const int wls_ub_pin = 6;

boolean wls_start = false;

int wls_waterlevel; // waterlevel = -1 + lb_state + ub_state
                    // -1: below lb; 0: between lb and ub; 1: above ub

void setup() {
    // Initialize Serial object
    Serial.begin(9600);

    // Set up sensor pins with pull-up resistors
    pinMode(wls_lb_pin, INPUT_PULLUP);
    pinMode(wls_ub_pin, INPUT_PULLUP);
    digitalWrite(wls_lb_pin, HIGH);
    digitalWrite(wls_ub_pin, HIGH);
}

void wls_control() {
    
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
        
        wls_start = true;
        
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

        //Return water level
        wls_waterlevel = -1 + wls_lb_read + wls_ub_read;
    }
}

void loop() {
    wls_control();
    if (wls_start == true) {
        Serial.println(wls_waterlevel);
        delay(500);
    }
}