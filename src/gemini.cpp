// #include "gemini.h"

// String gemini_address = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + String(api_key);
// String text_request_base = "I have 4 coloured pencils of the following rgb values: ";
// String text_request_top = ". What could a child draw using these colours? Please give me 3 options in an ordered list saying nothing else. They could be scenes, things, animals, etc. Please also respond in a simplistic manner with no adjectives. ONLY MENTION THE ITEMS, DO NOT MENTION THEIR COLOUR";
// String image_request_base = "Please image generate an outline of: ";

// void getDrawing(String prompt) {
// 	Serial.print("Image searching for: ");
// 	Serial.println(prompt);
// }

// void processText(String imageIdeas) {
//     int start = 0;
// 	int end = imageIdeas.indexOf('\n');

// 	while (end != -1) {
// 		String idea = imageIdeas.substring(start, end);
// 		idea = idea.substring(idea.indexOf(". ") + 2);

// 		getDrawing(idea);

// 		start = end + 1;
// 		end = imageIdeas.indexOf('\n', start);
// 	}

// 	String lastIdea = imageIdeas.substring(start);
// 	lastIdea = lastIdea.substring(lastIdea.indexOf(". ") + 2);
// 	getDrawing(lastIdea);
// }

// void genOutlines(String rgbInput) {
// 	HTTPClient http_client;
// 	// Check if the client can connect to the Gemini API.
// 	if (!http_client.begin(gemini_address)) {
// 		Serial.println("Connection to API failed!");
// 	}

// 	// Create the request body for the Gemini API.
// 	http_client.addHeader("Content-Type", "application/json");
// 	String requestBody = "{\"contents\": [{\"parts\":[{\"text\": \"" + text_request_base + rgbInput + text_request_top + "\"}]}]}";

// 	// Send the request to the Gemini API and check if the response is successful.
// 	int httpCode = http_client.POST(requestBody);
// 	if (httpCode != 200) {
// 		Serial.println("Failed to send request to Gemini");
// 		Serial.println(httpCode);
// 	}

// 	// Parse the response from the Gemini API and extract the advice.
// 	String response = http_client.getString();
// 	JsonDocument jsonDoc;
// 	deserializeJson(jsonDoc, response);
// 	String imageDescription = jsonDoc["candidates"][0]["content"]["parts"][0]["text"];
// 	processText(imageDescription);

// }