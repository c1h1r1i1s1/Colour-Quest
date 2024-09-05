#include <WiFi.h>

const char* ssid = "ESP32_Chris";
const char* password = "123456789"; // Set the password for the WiFi network.

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  WiFi.softAP(ssid, password);
  Serial.println("Wi-Fi AP started");

  server.begin();
}

void loop(){
  delay(1000); // Wait for 1 second
  int numClients = WiFi.softAPgetStationNum();
  Serial.print("Number of devices connected: ");
  Serial.println(numClients);
}

void send() {
  WiFiClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        String message = client.readStringUntil('\n');
        Serial.println("Received: " + message);
      }
    }
    client.stop();
  }
}