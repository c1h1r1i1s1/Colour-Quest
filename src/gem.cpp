#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <analogWrite.h>

// Initialize the HTTP client to send requests to the Gemini API.
HTTPClient http_client;
WiFiServer wifi_server(80);
// WiFi credentials and device name.
const char* ssid = "Blue";
const char* password = "Treetop1";
const char* device_name = "Tree";

// Gemini API key and server URL.
const char* api_key = "AIzaSyBNgeZKlT7KrfdV5Xz7dbfcTyTeR334bmw";
String gemini_server = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + String(api_key);
String project_description = "Imagine you are an aircon system. I will give you a prompt describing the temperature of the room felt by the inhabitant, and you will tell me to turn a fan up, down, or leave it the same. Please only respond with a number. 1 meaning turn the fan on, 0 meaning turn the fan off, or 2 meaning the temperature is ok and no change is required. The quote is as follows: ";
// String project_description = "Imagine you are a smart aircon system. I will give you a prompt describing the temperature felt by the inhabitant, \
    and you will return two booleans in JSON format: {\"fan_state\": 1/0, \"comfort_status\": 1/0} \
    If the temperature is too hot, you set the fan state to 1. If the temperature is ok, then set the comfort_status as 1. \
    If the temperature is too cold, set both to 0";

const char* temperature_descriptions[3] = {"It is too cold", "It is too hot", "It is just right"};
int current_description = 0;

String sendToGemini(String input) {
  // Check if the client can connect to the Gemini API.
  if (!http_client.begin(gemini_server)) {
    Serial.println("Connection to API failed!");
    return "";
  }
  
  // Create the request body for the Gemini API.
  http_client.addHeader("Content-Type", "application/json");
  String requestBody = "{\"contents\": [{\"parts\":[{\"text\": \"" + project_description + input + "\"}]}]}";
  
  // Send the request to the Gemini API and check if the response is successful.
  int httpCode = http_client.POST(requestBody);
  if (httpCode != 200) {
    Serial.println("Failed to send request to Gemini");
    Serial.println(httpCode);
    return "";
  }

  // Parse the response from the Gemini API and extract the advice.
  String response = http_client.getString();
  StaticJsonDocument<1024> jsonDoc;
  deserializeJson(jsonDoc, response);
  String advice = jsonDoc["candidates"][0]["content"]["parts"][0]["text"];
  return advice;
}
void processResponse(String advice) {
  if (advice.toInt() == 1) {
    digitalWrite(D2, LOW);
  } else if (advice.toInt() == 0) {
    digitalWrite(D2, LOW);
  } else {
    digitalWrite(D2, HIGH);
  }
}

void setup() {
  // Initialize the serial monitor.
  Serial.begin(9600);
  // Connect to the Wi-Fi network.
  // WiFi.setHostname(device_name);
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting to Wi-Fi...");
  // }
  
  // Serial.println("Connected to Wi-Fi");
  // Serial.print("IP Address: ");
  // Serial.println(WiFi.localIP());
  // // Start the server to listen for requests from the MCU-2.
  // wifi_server.begin();
  // // Set wait time for HTTP requests.
  // http_client.setTimeout(15000);
  
  pinMode(D2, OUTPUT);
  pinMode(4, OUTPUT);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    String geminiResponse = sendToGemini(String(temperature_descriptions[current_description]));
    // Send the request to the Gemini API.
    // Temporary: Print the response from the Gemini API.
    Serial.print("Temperature Description: ");
    Serial.println(temperature_descriptions[current_description]);
    Serial.println("Advice: " + geminiResponse);
    // TODO: Process the response from the Gemini API.
    // processResponse(geminiResponse);
    current_description = (current_description + 1) % 3; // Cycle through the temperature descriptions.
  }
  digitalWrite(D2, HIGH);
  analogWrite(4, 0);
  
  delay(2000);

  digitalWrite(D2, LOW);
  analogWrite(4, 200);

  delay(1000);
  // TODO: Comment the above code, read the request from the MCU-2 through WiFi and send it to Gemini API.
  // WiFiClient client = wifi_server.available();
  // if (client && client.connected() && client.available()) {
  // String request = client.readStringUntil('\r');
  // client.flush();
  // String geminiResponse = sendToGemini(request);
  // // TODO: Process the response from the Gemini API.
  // processResponse(geminiResponse);
  // client.stop();
  // }
}