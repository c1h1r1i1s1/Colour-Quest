#include "webServer.h"

const char* ssid = "Colour Quest";
const char* password = "123456789";

AsyncWebServer server(80);

String difficulty;
String colourBlindMode;

void loadSettings() {
	File file = SPIFFS.open("/settings.json");
	if (!file) {
		Serial.println("Failed to open settings file for reading");
		return;
	}

	// Create a JSON document to store the read values
	JsonDocument doc;
	DeserializationError error = deserializeJson(doc, file);
	if (error) {
		Serial.println("Failed to parse settings file");
		return;
	}

	// Retrieve settings from JSON
	difficulty = doc["difficulty"] | "Easy";  // Use "easy" as default
	colourBlindMode = doc["colourBlindMode"] | "None";

	file.close();
}

bool saveSettings() {
	// Open file for writing
	File file = SPIFFS.open("/settings.json", FILE_WRITE);
	if (!file) {
		Serial.println("Failed to open settings file for writing");
		return 0;
	}

	// Create a JSON document to hold the settings
	JsonDocument doc;
	doc["difficulty"] = difficulty;
	doc["colourBlindMode"] = colourBlindMode;

	// Serialize JSON to the file
	serializeJson(doc, file);
	file.close();

	return 1;
}

void handleSubmit(AsyncWebServerRequest *request) {
	if (request->hasParam("difficulty", true) && request->hasParam("colour-blind-mode", true)) {
		difficulty = request->getParam("difficulty", true)->value();
		colourBlindMode = request->getParam("colour-blind-mode", true)->value();

		// Debug print to Serial
		Serial.println("\nReceived POST request:");
		Serial.println("Difficulty: " + difficulty);
		Serial.println("Colour Blind Mode: " + colourBlindMode);

		// Save settings and display status page
		if (saveSettings()) {
			request->send(SPIFFS, "/success.html", "text/html");
		} else {
			request->send(SPIFFS, "/failure.html", "text/html");
		}

	} else {
		Serial.println("Error with setting change submission");
		request->send(400, "text/plain", "Missing parameters");
	}
}

void setupWebServer() {
	// Start wifi access point
	WiFi.softAP(ssid, password);
	Serial.println("Wi-Fi AP started");

	// Initialize SPIFFS for web page file handling
	if (!SPIFFS.begin(true)) {
		Serial.println("An error occurred while mounting SPIFFS");
		return;
	}

	// Load user settings
	loadSettings();

	// Serve static files from SPIFFS
	server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

	// Setup pathways
	// Handle form submission via POST
	server.on("/submit", HTTP_POST, [](AsyncWebServerRequest *request) {
		handleSubmit(request);
	});

	server.on("/get-settings", HTTP_GET, [](AsyncWebServerRequest *request){
		String json = "{\"difficulty\": \"" + difficulty + "\", \"colourBlindMode\": \"" + colourBlindMode + "\"}";
		request->send(200, "application/json", json);
	});

	// Serve the favicon.ico
	server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(SPIFFS, "/favicon.ico", "image/x-icon");
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