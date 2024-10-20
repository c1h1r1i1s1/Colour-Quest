#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Replace with your WiFi credentials
const char* ssid = "moyuing";
const char* password = "000918927";

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) { // Check Wi-Fi connection status
    HTTPClient http;
    
    // Your Google Cloud TTS API endpoint
    String url = "https://texttospeech.googleapis.com/v1/text:synthesize?key=AIzaSyDhxVkeF1MJvMuGrjT4BcovRg8HviN1LkE";

    // Construct the request payload
    StaticJsonDocument<512> doc;
    doc["input"]["text"] = "Brevity is the soul of wit.";
    doc["voice"]["languageCode"] = "en-US";
    doc["voice"]["name"] = "en-US-Wavenet-D"; // Example of an English voice
    doc["audioConfig"]["audioEncoding"] = "MP3";

    String requestBody;
    serializeJson(doc, requestBody);

    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    // Send the POST request
    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Request succeeded, response:");
      Serial.println(response);
    } else {
      Serial.print("Error in request, error code: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Close the connection
  } else {
    Serial.println("WiFi not connected");
  }

  delay(10000); // Wait for 10 seconds before making the next request
}
