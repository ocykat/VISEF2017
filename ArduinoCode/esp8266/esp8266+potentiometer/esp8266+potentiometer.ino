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
const int sll_port = 443;

// Server
const String SERVER = "tester1605.000webhostapp.com";

String esp_response;
bool esp_ReplyOK;

// Access point - Wifi network
// String SSID = "DLINK_WIRELESS";
// String PASSWORD = "1234567890";
String SSID = "Danato Alzheim's Phone";
String PASSWORD = "12345678";

// Potentiometer
const int potentiometer_vcc_pin = 9;
const int potentiometer_Aout_pin = A0;

int anlval;

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

	// Potentiometer
	pinMode(potentiometer_vcc_pin, OUTPUT);
	digitalWrite(potentiometer_vcc_pin, HIGH);

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
	esp_SendCmd("AT+CIPMUX=0", "OK", "_EMPTY_", 1, 1000);
	esp_CheckResult("AT+CIPMUX=0", "TCP/UDP connections: Single");
}

void loop() {
	// Start a TCP connection
	String start_TCP_cn_cmd = "AT+CIPSTART=\"TCP\",\"" + SERVER + "\"," + MAINPORT;
	esp_SendCmd(start_TCP_cn_cmd, "OK", "_EMPTY_", 1, 3000);
	esp_CheckResult(start_TCP_cn_cmd, "Connected to server!");
	
	// Send message
	anlval = analogRead(A0);
	String data = "GET /receiver.php?anlinp=" + String(anlval) + " HTTP/1.1\r\nHost: " + SERVER + "\r\n\r\n";
	Serial.println("*****Analog input: " + String(anlval) + "*****");
	String data_StringLength = String(data.length());
	String html_cmd = "AT+CIPSEND=" + data_StringLength;
	esp_SendCmd(html_cmd, ">", "_EMPTY_", 1, 1000);
	esp_CheckResult("AT+CIPSEND", "Sending...");
	if (esp_ReplyOK) {
		esp.println(data);
		Serial.println("____SENT!!____");
	}

	// Close the connection
	esp_SendCmd("AT+CIPCLOSE", "OK", "ERROR", 1, 1000);
	Serial.println("Connection closed!");
	delay(5000);
}