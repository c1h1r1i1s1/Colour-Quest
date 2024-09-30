#include "gemini.h"

String gemini_address = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + String(api_key);
String text_request_base = "I have 4 coloured pencils of the following rgb values: ";
String text_request_top = ". What could a child draw using these colours? Please give me 3 options in an ordered list saying nothing else. They could be scenes, things, animals, etc. Please also respond in a simplistic manner with no adjectives. ONLY MENTION THE ITEMS, DO NOT MENTION THEIR COLOUR";
String image_request_base = "Please image generate an outline of: ";

void genOutlines(String rgbInput) {
	HTTPClient http_client;
	// Check if the client can connect to the Gemini API.
	if (!http_client.begin(gemini_address)) {
		Serial.println("Connection to API failed!");
	}

	// Create the request body for the Gemini API.
	http_client.addHeader("Content-Type", "application/json");
	String requestBody = "{\"contents\": [{\"parts\":[{\"text\": \"" + text_request_base + rgbInput + text_request_top + "\"}]}]}";

	// Send the request to the Gemini API and check if the response is successful.
	int httpCode = http_client.POST(requestBody);
	if (httpCode != 200) {
		Serial.println("Failed to send request to Gemini");
		Serial.println(httpCode);
	}

	// Parse the response from the Gemini API and extract the advice.
	String response = http_client.getString();
	JsonDocument jsonDoc;
	deserializeJson(jsonDoc, response);
	String imageDescription = jsonDoc["candidates"][0]["content"]["parts"][0]["text"];
	Serial.println(imageDescription);
}