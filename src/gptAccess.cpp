#include "gptAccess.h"

void downloadImage(String imageUrl, String filePath) { // Not using anymore because it is extremely slow to download and breaks
	HTTPClient http;
	http.setTimeout(30000);
    http.begin(imageUrl);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        File file = SPIFFS.open(filePath, FILE_WRITE);
        if (!file) {
            Serial.println("Failed to open file in SPIFFS");
            http.end();
            return;
        }

        WiFiClient* stream = http.getStreamPtr();
        uint8_t buffer[512];
		size_t totalBytes = http.getSize();
		size_t downloadedBytes = 0;

		// Read data from the server and write to the file
		while (http.connected() && (stream->available() > 0 || downloadedBytes < totalBytes)) {
			size_t bytesRead = stream->readBytes(buffer, sizeof(buffer));
			if (bytesRead > 0) {
				// size_t n = stream->readBytes(buffer, sizeof(buffer));
				file.write(buffer, bytesRead);
				downloadedBytes += bytesRead;
			}
		}

        file.close();
        Serial.println("Download completed and saved to SPIFFS.");
        http.end();
    } else {
        Serial.printf("HTTP GET request failed, code: %d\n", httpCode);
        http.end();
    }
}

String generateImage(String prompt) {
	const char* host = "api.openai.com";
	const int httpsPort = 443;

	WiFiClientSecure client;
	client.setInsecure();

	if (!client.connect(host, httpsPort)) {
		Serial.println("Connection failed");
		return "";
	}

	String url = "/v1/images/generations";
	String auth_header = "Bearer " + String(gpt_personal);
	String requestBody = "{\"prompt\": \"Create a simple line-illustration, suitable as a colouring-in page for children. The illustration should be of: " + prompt + "\", \"size\": \"1024x1024\", \"model\": \"dall-e-3\"}";

	client.println("POST " + url + " HTTP/1.1");
	client.println("Host: " + String(host));
	client.println("Authorization: " + auth_header);
	client.println("Content-Type: application/json");
	client.print("Content-Length: ");
	client.println(requestBody.length());
	client.println();
	client.println(requestBody);

	// Wait for the response
	while (client.connected()) {
		String line = client.readStringUntil('\n');
		if (line == "\r") {
			Serial.println("Headers received");
			break;
		}
	}

	// Read the response
	String response = client.readString();
	Serial.println(response);
	JsonDocument jsonDoc;
	deserializeJson(jsonDoc, response);
	String imageUrl = jsonDoc["data"][0]["url"];

	return imageUrl;
}

void createImages(String colours) {
	const char* host = "api.openai.com";
	const int httpsPort = 443;

	WiFiClientSecure client;
	client.setInsecure();

	if (!client.connect(host, httpsPort)) {
		Serial.println("Connection failed");
		return;
	}

	String url = "/v1/chat/completions";
	String auth_header = "Bearer " + String(gpt_personal);
	String requestBody = R"({
		"model": "gpt-4o-mini",
		"messages": [
			{
				"role": "system",
				"content": "You are tasked with recommending things for a child to draw based on user-provided RGB colours. You will be given a list of RGB colours and shall return 3 image options in the json format provided, saying nothing else. They could be scenes, things, animals etc. Please respond in a simplistic manner without mentioning any colours and using minimal adjectives. DO NOT MENTION THE INTENDED COLOURS OF THE ELEMENTS OF THE DRAWING. Just list the elements of the drawing WITHOUT REFERENCING THEIR COLOUR."
			},
			{
				"role": "user",
				"content": ")" + colours + R"("
			}
		],
		"response_format": {
			"type": "json_schema",
			"json_schema": {
				"name": "image_recommendations",
				"schema": {
					"type": "object",
					"properties": {
						"recommendations": {
							"type": "array",
							"items": {
								"type": "object",
								"properties": {
									"idea": { "type": "string" }
								},
								"required": ["idea"],
								"additionalProperties": false
							}
						}
					},
					"required": ["recommendations"],
					"additionalProperties": false
				},
				"strict": true
			}
		}
	})";

	client.println("POST " + url + " HTTP/1.1");
	client.println("Host: " + String(host));
	client.println("Authorization: " + auth_header);
	client.println("Content-Type: application/json");
	client.print("Content-Length: ");
	client.println(requestBody.length());
	client.println();
	client.println(requestBody);

	// Wait for the response
	while (client.connected()) {
		String line = client.readStringUntil('\n');
		if (line == "\r") {
			Serial.println("Headers received");
			break;
		}
	}

	// Read the response
	String response = client.readString();
	Serial.println(response);
	JsonDocument jsonDoc;
	deserializeJson(jsonDoc, response);
	String stringRecommendations = jsonDoc["choices"][0]["message"]["content"];
	Serial.println(stringRecommendations);
	JsonDocument recommendations;
	deserializeJson(recommendations, stringRecommendations);
	for (int i=0; i<3; i++) {
		String idea = recommendations["recommendations"][i]["idea"];
		String imageURL = generateImage(idea);
		saveURL(imageURL);
	}
}