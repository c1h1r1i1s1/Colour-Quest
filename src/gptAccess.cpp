#include "gptAccess.h"

void generateImage(String prompt) {
	// const char* host = "api.openai.com";
	// const int httpsPort = 443;

	// WiFiClientSecure client;
	// client.setInsecure();

	// if (!client.connect(host, httpsPort)) {
	// 	Serial.println("Connection failed");
	// 	return;
	// }

	// String url = "/v1/images/generations";
	// String auth_header = "Bearer " + String(gpt_personal);
	// String requestBody = "{\"prompt\": \"" + prompt + "\", \"size\": \"1024x1024\", \"model\": \"dall-e-3\"}";

	// client.println("POST " + url + " HTTP/1.1");
	// client.println("Host: " + String(host));
	// client.println("Authorization: " + auth_header);
	// client.println("Content-Type: application/json");
	// client.print("Content-Length: ");
	// client.println(requestBody.length());
	// client.println();
	// client.println(requestBody);

	// // Wait for the response
	// while (client.connected()) {
	// 	String line = client.readStringUntil('\n');
	// 	if (line == "\r") {
	// 		Serial.println("Headers received");
	// 		break;
	// 	}
	// }

	// // Read the response
	// String response = client.readString();
	// Serial.println(response);
	// JsonDocument jsonDoc;
	// deserializeJson(jsonDoc, response);
	// String imageUrl = jsonDoc["data"][0]["url"];

	const char* url = "https://oaidalleapiprodscus.blob.core.windows.net/private/org-3RoLMbZBZSrDfeH4K8GlrGAS/user-EmIBImSaT42io42I320A4T7L/img-llJ1P9UPTQyVGgtKDBBECPWa.png?st=2024-10-10T08%3A59%3A06Z&se=2024-10-10T10%3A59%3A06Z&sp=r&sv=2024-08-04&sr=b&rscd=inline&rsct=image/png&skoid=d505667d-d6c1-4a0a-bac7-5c84a87759f8&sktid=a48cca56-e6da-484e-a814-9c849652bcb3&skt=2024-10-09T23%3A01%3A48Z&ske=2024-10-10T23%3A01%3A48Z&sks=b&skv=2024-08-04&sig=I1nRzun5F8efkeUpQ9f99iBi/42ouLRE9daY2ltgVUY%3D";
	const char* filePath = "/dragon.png";

	HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        File file = SPIFFS.open(filePath, FILE_WRITE);
        if (!file) {
            Serial.println("Failed to open file in SPIFFS");
            http.end();
            return;
        }

        WiFiClient* stream = http.getStreamPtr();
        uint8_t buffer[128] = { 0 };
        size_t size = 0;

        // Read data from the server and write to the file
        while (http.connected() && (size = stream->available())) {
            size = stream->readBytes(buffer, sizeof(buffer));
            file.write(buffer, size);
        }

        file.close();
        Serial.println("Download completed and saved to SPIFFS.");
        http.end();
    } else {
        Serial.printf("HTTP GET request failed, code: %d\n", httpCode);
        http.end();
    }




	// Serial.println("starting");

	// String imageUrl = "https://oaidalleapiprodscus.blob.core.windows.net/private/org-3RoLMbZBZSrDfeH4K8GlrGAS/user-EmIBImSaT42io42I320A4T7L/img-llJ1P9UPTQyVGgtKDBBECPWa.png?st=2024-10-10T08%3A59%3A06Z&se=2024-10-10T10%3A59%3A06Z&sp=r&sv=2024-08-04&sr=b&rscd=inline&rsct=image/png&skoid=d505667d-d6c1-4a0a-bac7-5c84a87759f8&sktid=a48cca56-e6da-484e-a814-9c849652bcb3&skt=2024-10-09T23%3A01%3A48Z&ske=2024-10-10T23%3A01%3A48Z&sks=b&skv=2024-08-04&sig=I1nRzun5F8efkeUpQ9f99iBi/42ouLRE9daY2ltgVUY%3D";

	// Serial.println("about to print");

	// // Send HTTP GET request
	// client.print(String("GET ") + imageUrl + " HTTP/1.1\r\n" +
	// 		"Host: api.openai.com\r\n" +
	// 		"Connection: close\r\n\r\n");

	// Serial.println("waiting for available");

	// while (client.connected() && !client.available()) delay(10);

	// Serial.println("about to read string");

	// while (client.available()) {
	// 	String line = client.readStringUntil('\n');
	// 	if (line == "\r") break;
	// }

	// Serial.println("about to open file");

	// File file = SPIFFS.open("/test.png", FILE_WRITE);
	// if (!file) {
	// 	Serial.println("Failed to open file for writing");
	// 	return;
	// }

	// Serial.println("about to read client bytes");

	// while (client.available()) {
	// 	uint8_t buffer[128];
	// 	int bytesRead = client.read(buffer, sizeof(buffer));
	// 	file.write(buffer, bytesRead);
	// }

	// Serial.println("Saved image to /test.png");
	// file.close();

	// client.stop();  // Close the connection
	// WiFiClientSecure client;
	// client.setInsecure();  // Use for testing. Prefer setCACert() for production.
	// client.setTimeout(150000);  // Set timeout for connection
	// HTTPClient http_client;

	// String gpt_endpoint = "https://api.openai.com/v1/images/generations";
	// String auth_header = "Bearer " + String(gpt_personal);

	// // Check if the client can connect to the Gemini API.
	// if (!http_client.begin(client, gpt_endpoint)) {
	// 	Serial.println("Connection to API failed!");
	// }

	// // Create the request body for the Gemini API.
	// http_client.addHeader("Content-Type", "application/json");
	// http_client.addHeader("Authorization", auth_header);

	// String requestBody = "{\"prompt\": \"" + prompt + "\", \"size\": \"1024x1792\", \"model\": \"dall-e-3\"}";
	// Serial.println(requestBody);

	// // Send the request to the Gemini API and check if the response is successful.
	// int httpCode = http_client.POST(requestBody);
	// if (httpCode != 200) {
	// 	Serial.println("Failed to send request");
	// 	Serial.println(httpCode);
	// }

	// Serial.println("about to get string");

	// String response = http_client.getString();
	// JsonDocument jsonDoc;
	// deserializeJson(jsonDoc, response);

	// Serial.println("printing");

	// Serial.println(response);
}