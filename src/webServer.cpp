#include "webServer.h"

const char* ap_ssid = "Colour Quest";
const char* ap_password = "123456789";

AsyncWebServer server(80);

void handleSubmit(AsyncWebServerRequest *request) {
	if (request->hasParam("difficulty", true) && request->hasParam("colour-blind-mode", true) && request->hasParam("ssid", true) && request->hasParam("password", true)) {
		String difficulty = request->getParam("difficulty", true)->value();
		String colourBlindMode = request->getParam("colour-blind-mode", true)->value();
		String ssid = request->getParam("ssid", true)->value();
		String password = request->getParam("password", true)->value();

		// Debug print to Serial
		Serial.println("\nReceived POST request:");
		Serial.println("Difficulty: " + difficulty);
		Serial.println("Colour Blind Mode: " + colourBlindMode);
		Serial.println("SSID: " + ssid);
		Serial.println("Password: " + password);

		// Save settings and display status page
		if (saveSettings(difficulty, colourBlindMode, ssid, password)) {
			request->send(SPIFFS, "/success.html", "text/html");
		} else {
			request->send(SPIFFS, "/failure.html", "text/html");
		}

	} else {
		Serial.println("Error with setting change submission");
		request->send(400, "text/plain", "Missing parameters");
	}
}

void setupWebServer(String difficulty, String colourBlindMode) {
	// Start wifi access point
	WiFi.softAP(ap_ssid, ap_password);
	Serial.println("Wi-Fi AP started");

	// Serve static files from SPIFFS
	server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

	// Setup pathways
	// Handle form submission via POST
	server.on("/submit", HTTP_POST, [](AsyncWebServerRequest *request) {
		handleSubmit(request);
	});

	server.on("/get-settings", HTTP_GET, [](AsyncWebServerRequest *request){
		String json = "{\"difficulty\": \"" + getDifficulty() + "\", \"colourBlindMode\": \"" + getColourBlindMode() + "\", \"ssid\": \"" + getSsid() + "\", \"password\": \"" + getPassword() + "\"}";
		request->send(200, "application/json", json);
	});

	// Serve the favicon.ico
	server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(SPIFFS, "/favicon.ico", "image/x-icon");
	});

	server.on("/get-image-urls", HTTP_GET, [](AsyncWebServerRequest *request){
		String doc = getURLs();
		request->send(200, "application/json", doc);
	});

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