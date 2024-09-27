#include "fileAccess.h"

String difficulty;
String colourBlindMode;
String ssid;
String password;

void loadSettings() {
	// Initialize SPIFFS for web page file handling
	if (!SPIFFS.begin(true)) {
		Serial.println("An error occurred while mounting SPIFFS");
		return;
	}
	
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
	difficulty = doc["difficulty"] | "EASY";
	colourBlindMode = doc["colourBlindMode"] | "NONE";
	ssid = doc["ssid"] | "ssid";
	password = doc["password"] | "password";

	file.close();
}

String getDifficulty() {
	return difficulty;
}

String getColourBlindMode() {
	return colourBlindMode;
}

String getSsid() {
	return ssid;
}

String getPassword() {
	return password;
}

bool saveSettings(String newDifficulty, String newColourBlindMode, String newSsid, String newPassword) {
	// Open file for writing
	File file = SPIFFS.open("/settings.json", FILE_WRITE);
	if (!file) {
		Serial.println("Failed to open settings file for writing");
		return 0;
	}

	// Create a JSON document to hold the settings
	JsonDocument doc;
	doc["difficulty"] = newDifficulty;
	doc["colourBlindMode"] = newColourBlindMode;
	doc["ssid"] = newSsid;
	doc["password"] = newPassword;

	difficulty = newDifficulty;
	colourBlindMode = newColourBlindMode;
	ssid = newSsid;
	password = newPassword;

	// Serialize JSON to the file
	serializeJson(doc, file);
	file.close();

	return 1;
}