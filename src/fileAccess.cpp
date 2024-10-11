#include "fileAccess.h"

String difficulty;
String colourBlindMode;
String ssid;
String password;

bool checkTimeout(const char* timeStamp) {
	configTime(0, 0, "pool.ntp.org");
    
    time_t now = time(nullptr);
    // struct tm *timeinfo = localtime(&now);
    
    // char currentTime[80];
    // strftime(currentTime, sizeof(currentTime), "%Y-%m-%d %H:%M:%S", timeinfo);

    struct tm tm;
	if (strptime(timeStamp, "%Y-%m-%d %H:%M:%S", &tm)) {
		time_t urlTime = mktime(&tm); // Convert tm structure to time_t
		// Check if the timestamp is within the last hour (3600 seconds)
		if (difftime(now, urlTime) <= 3600) {
			return 1;
		} else {
			return 0;
		}
	} else {
		Serial.println("Failed to parse timestamp");
		return 0;
	}
}

String getURLs() {
	JsonDocument doc;
	File file = SPIFFS.open("/imageURLs.json");
	if (!file) {
		Serial.println("Failed to open settings file for reading");
		return "{}";
	}

	DeserializationError error = deserializeJson(doc, file);
	if (error) {
		Serial.println("Failed to parse URL file");
		return "{}";
	}
	file.close();

	JsonDocument resultDoc; // Adjust size as necessary
    JsonArray urls = resultDoc["urls"].to<JsonArray>();

	for (JsonVariant urlVariant : doc["urls"].as<JsonArray>()) {
		JsonObject urlObj = urlVariant.as<JsonObject>();
		const char* url = urlObj["url"];
		const char* timestamp = urlObj["timestamp"];

		if (checkTimeout(timestamp)) {
			JsonObject newUrlObj = urls.add<JsonObject>();
			newUrlObj["url"] = url;
			newUrlObj["timestamp"] = timestamp;
		}
	}

	String jsonResponse;
	serializeJson(resultDoc, jsonResponse);
	return jsonResponse;
}

void saveURL(String url) {
	// First get timestamp as the link expires after 60 min
	configTime(0, 0, "pool.ntp.org");
    
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    Serial.println("Current Timestamp: " + String(buffer));

	File file = SPIFFS.open("/imageURLs.json");
	if (!file) {
		Serial.println("Failed to open settings file for reading");
		return;
	}

	JsonDocument doc;
	DeserializationError error = deserializeJson(doc, file);
	if (error) {
		Serial.println("Failed to parse URL file");
		return;
	}

	file.close();

	JsonObject newUrlEntry = doc["urls"].add<JsonObject>();
	newUrlEntry["url"] = url;
	newUrlEntry["timestamp"] = String(buffer);

	File newFile = SPIFFS.open("/imageURLs.json", FILE_WRITE);
	if (!newFile) {
		Serial.println("Failed to open settings file for writing");
		return;
	}

	serializeJson(doc, newFile);

	newFile.close();
}

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