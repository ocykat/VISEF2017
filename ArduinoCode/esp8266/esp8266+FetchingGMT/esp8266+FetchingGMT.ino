#include <SoftwareSerial.h>

// Pins
const int esp_rx_pin = 6;
const int esp_tx_pin = 7;

// ESP8266 object
SoftwareSerial esp(esp_tx_pin, esp_rx_pin);

// Baudrate
const int Serial_baudrate = 9600;
const int esp_baudrate = 9600;

// Ports
const int MAINPORT = 80;
const int SLLPORT = 443;

// Server
const String SERVER = "api.thingspeak.com";
const String GET_request = "GET apps/thinghttp/send_request?api_key=XIJNR7W5GQF4PZ0B HTTP/1.1\r\nHost: api.thingspeak.com\r\n\r\n\r\n";

String esp_response;
bool esp_ReplyOK;

// Access point - Wifi network
String SSID = "FITA-HOTEL";
String PASSWORD = "fita2017";

void esp_flush() {
	while (esp.available() > 0) {
		esp.read();
	}
}

void esp_SendCmd(String cmd, String reply1, String reply2, int repeattime, int delaytime) {
	for (int i = 0; i < repeattime; i++) {
		esp_response = "";
		esp.println(cmd);
		delay(150);
		while (esp.available() > 0) {
			char c = esp.read();
			esp_response += c;
		}
		if ((esp_response.indexOf(reply1) != -1) || (esp_response.indexOf(reply2) != -1)) {
			esp_ReplyOK = true;
		}
		else {
			esp_ReplyOK = false;		
		}
		esp_flush();
		delay(delaytime);
	}
}

void esp_CheckResult(String cmd, String NoErrorMessage) {
	Serial.print(cmd + ": ");
	if (esp_ReplyOK) {
		Serial.println(NoErrorMessage);
	}
	else {
		Serial.println("ERROR!!!");
	}
}

void setup() {
	Serial.begin(Serial_baudrate);
	esp.begin(esp_baudrate);

	// Setup LED
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);

	// Reset ESP8266
	esp_SendCmd("AT+RST", "OK", "_EMPTY_", 1, 1000);
	esp_CheckResult("AT+RST", "ESP has been reset!");

	// Check if ESP8266 is running
	esp_SendCmd("AT", "OK", "_EMPTY_", 3, 1000);
	esp_CheckResult("AT", "ESP is running");

	// Set ESP8266 to client (STA) mode
	esp_SendCmd("AT+CWMODE=1", "OK", "change", 1, 5000);
	esp_CheckResult("AT+CWMODE=1", "ESP is set to STA mode!");

	// Join the given Access Point
	String join_AP_cmd = "AT+CWJAP=\"" + SSID + "\",\"" + PASSWORD + "\"";
	Serial.println(join_AP_cmd);
	esp_SendCmd(join_AP_cmd, "OK", "change", 2, 10000);
	esp_CheckResult(join_AP_cmd, "Successfully connected to " + SSID);

	// TCP/UDP connections: Single
	esp_SendCmd("AT+CIPMUX=0", "OK", "_EMPTY_", 1, 4000);
	esp_CheckResult("AT+CIPMUX=0", "TCP/UDP connections: Single");
}

void loop() {
	// Start a TCP connection
	esp_ReplyOK = false;
	while (!esp_ReplyOK) {
		String start_TCP_cn_cmd = "AT+CIPSTART=\"TCP\",\"" + SERVER + "\"," + MAINPORT;
		esp_SendCmd(start_TCP_cn_cmd, "OK", "_EMPTY_", 1, 5000);
		// esp_CheckResult(start_TCP_cn_cmd, "Connected to server!");
		Serial.println("Connecting to server...");
	}
	
	if (esp_ReplyOK) {
		// Send message
		Serial.println("______________________________________");
		Serial.println("*****Connected to server!*****");
		String GET_request_cmd = "AT+CIPSEND=" + String(GET_request.length());
		esp_SendCmd(GET_request_cmd, ">", "_EMPTY_", 1, 2000);
		if (esp_ReplyOK) {
			String received_data = "";
			esp.println(GET_request);
			Serial.println("Sending GET request!");
			delay(200);
			long time_recorder = millis();
			if (esp.find("Date:")) {
				Serial.println("Data Received!!!!!!!!!!!");
				digitalWrite(13, !digitalRead(13));
				for (int i = 0; i < 500; i++) {
					while (esp.available() > 0) {
						char c = esp.read();
						received_data += c;
					}
					// if (received_data.indexOf("GMT" != -1)) {
					// 	break;
					// }
					// delay(500);
					if (millis() - time_recorder > 5000) break;
				}
				// String DateAndTime = ;
				Serial.println("______________________________________");
				Serial.println("Date and Time: " + received_data.substring(1, 30));
				Serial.println("______________________________________");
				delay(2000);
			}
			else {
				Serial.println("Data have not been received! Wait for next connection...");
			}
		}
		// Close the connection
		esp_SendCmd("AT+CIPCLOSE", "OK", "ERROR", 1, 1000);
		Serial.println("Connection closed!");
		delay(2000);
	}
	else {
		// Close the connection
		esp_SendCmd("AT+CIPCLOSE", "OK", "ERROR", 1, 1000);
		Serial.println("Connection closed!");
		delay(2000);		
	}
}