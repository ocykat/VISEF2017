#include <dht.h>

// DHT pin
const int dht_out_pin = 7;

// DHT object
dht DHT;

void setup() {
    Serial.begin(9600);
}

void loop() {
    int dht_check = DHT.read11(dht_out_pin);
    // switch (dht_check) {
    //  case DHTLIB_OK: {
    //      Serial.println("OK");
    //  }
    //  break;
    //  case DHTLIB_ERROR_CHECKSUM: {
    //      Serial.println("CheckSumError!!");
    //  }
    //  break;
    //  case DHTLIB_ERROR_TIMEOUT: {
    //      Serial.println("TimeOutError!!");
    //  }
    //  break;
    //  default: {
    //      Serial.println("UnknownError!");
    //  }
    // }

    Serial.println(DHT.temperature);
    Serial.println(DHT.humidity);
    Serial.println("");
    delay(2000);
}