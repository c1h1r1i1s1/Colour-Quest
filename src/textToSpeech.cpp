#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "api-key.h"

void setup() {
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
