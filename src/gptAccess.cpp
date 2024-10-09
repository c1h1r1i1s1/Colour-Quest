#include "gptAccess.h"

void generateImage(String prompt) {
	const char* host = "api.openai.com";
	const int httpsPort = 443;

	WiFiClientSecure client;
	client.setInsecure();

	if (!client.connect(host, httpsPort)) {
		Serial.println("Connection failed");
		return;
	}

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

	String imageUrl = "https://oaidalleapiprodscus.blob.core.windows.net/private/org-3RoLMbZBZSrDfeH4K8GlrGAS/user-EmIBImSaT42io42I320A4T7L/img-RxQd2xxWDfgp4ExIF6VbGQAd.png?st=2024-10-09T03%3A50%3A33Z&se=2024-10-09T05%3A50%3A33Z&sp=r&sv=2024-08-04&sr=b&rscd=inline&rsct=image/png&skoid=d505667d-d6c1-4a0a-bac7-5c84a87759f8&sktid=a48cca56-e6da-484e-a814-9c849652bcb3&skt=2024-10-08T23%3A41%3A28Z&ske=2024-10-09T23%3A41%3A28Z&sks=b&skv=2024-08-04&sig=8XMSQDH55pns3liVC0f8DLSt9THLpO/hlHmOxiW1AmU%3D";

	// Send HTTP GET request
	client.print(String("GET ") + imageUrl + " HTTP/1.1\r\n" +
			"Host: api.openai.com\r\n" +
			"Connection: close\r\n\r\n");

	while (client.connected() && !client.available()) delay(10);

	while (client.available()) {
		String line = client.readStringUntil('\n');
		if (line == "\r") break;
	}

	File file = SPIFFS.open("/test.jpg", FILE_WRITE);
	if (!file) {
		Serial.println("Failed to open file for writing");
		return;
	}

	while (client.available()) {
		uint8_t buffer[128];
		int bytesRead = client.read(buffer, sizeof(buffer));
		file.write(buffer, bytesRead);
	}

	Serial.println("Saved image to /test.jpg");
	file.close();

	client.stop();  // Close the connection
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