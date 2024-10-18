#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// Replace with your Wi-Fi credentials and API key
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* apiKey = "YOUR_GOOGLE_TTS_API_KEY";

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  // Prepare the request to Google TTS API
  WiFiClientSecure client;
  HTTPClient https;

  client.setInsecure(); // For now, ignore certificate verification
  
  String url = "https://texttospeech.googleapis.com/v1/text:synthesize?key=" + String(apiKey);
  
  String jsonPayload = "{\"input\":{\"text\":\"Hello, how are you?\"},"
                       "\"voice\":{\"languageCode\":\"en-US\",\"ssmlGender\":\"FEMALE\"},"
                       "\"audioConfig\":{\"audioEncoding\":\"MP3\"}}";
  
  https.begin(client, url);
  https.addHeader("Content-Type", "application/json");

  int httpCode = https.POST(jsonPayload);

  if (httpCode > 0) {
    String response = https.getString();
    Serial.println(response);  // You will extract the audioContent field from this JSON
    // Decode the base64 audioContent here
  } else {
    Serial.printf("HTTP request failed, error: %s\n", https.errorToString(httpCode).c_str());
  }

  https.end();
}

void loop() {
  // You can leave this empty for now
}
