#include "webServer.h"

const char* ssid = "Colour Quest";
const char* password = "123456789";
// WiFiServer server(80);
AsyncWebServer server(80);

void setupWebServer() {
	// Start wifi access point
	WiFi.softAP(ssid, password);
	Serial.println("Wi-Fi AP started");

	// Initialize SPIFFS for web page file handling
	if (!SPIFFS.begin(true)) {
		Serial.println("An error occurred while mounting SPIFFS");
		return;
	}

	// Start web server
	// server.begin();

	// Serve static files from SPIFFS
	server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

	// Start the server
	server.begin();
	Serial.println("HTTP server started");

	// Start dns resolver for website name (colourquest.local)
	if (!MDNS.begin("colourquest")) {
		Serial.println("Error starting mDNS");
		return;
	}
	Serial.println("mDNS responder started");

	// Announce the hostname on the network
	MDNS.addService("http", "tcp", 80);
}

void webListener() {
	// WiFiClient client = server.available();

	// if (client) {
	// 	Serial.println("New Client Connected");
	// 	String currentLine = "";

	// 	// Loop while the client's connected
	// 	while (client.connected()) {
	// 		if (client.available()) {
	// 			char c = client.read();
	// 			Serial.write(c); // Print request to serial monitor
	// 			currentLine += c;

	// 			// If the request ends with a new line, it's the end of the request
	// 			if (c == '\n') {
	// 				// Send a response
	// 				client.println("HTTP/1.1 200 OK");
	// 				client.println("Content-type:text/html");
	// 				client.println(); // end headers

	// 				// Web page content (simple HTML)
	// 				client.println("<!DOCTYPE html><html>");
	// 				client.println("<head><title>ColorQuest</title></head>");
	// 				client.println("<body><h1>Welcome to ColorQuest</h1>");
	// 				client.println("<p>Use this interface to control the device.</p>");
	// 				client.println("</body></html>");

	// 				// Break out of the loop
	// 				break;
	// 			}
	// 		}
	// 	}

	// 	// Give the web browser time to receive the data
	delay(1);

	// 	// Close the connection
	// 	client.stop();
	// 	Serial.println("Client Disconnected");
	// }
}