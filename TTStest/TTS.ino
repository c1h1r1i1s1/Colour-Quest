#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <driver/i2s.h>

// Wi-Fi credentials
const char* ssid = "moyuing";
const char* password = "000918927";

// Google TTS API URL
const char* api_key = "AIzaSyDhxVkeF1MJvMuGrjT4BcovRg8HviN1LkE"; 
String url = "https://texttospeech.googleapis.com/v1/text:synthesize?key=" + String(api_key);

// I2S configuration
#define I2S_NUM         I2S_NUM_0
#define I2S_BCK_IO      26
#define I2S_WS_IO       25
#define I2S_DO_IO       22

// Base64 decoding table
const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz"
                           "0123456789+/";

// Function to decode Base64
int base64_decode(uint8_t* output, const char* input, int input_len) {
  int output_len = 0;
  int i = 0;
  int j = 0;
  uint8_t temp[4];

  while (input_len-- && (input[j] != '=') && isBase64(input[j])) {
    temp[i++] = input[j++];
    if (i == 4) {
      output[output_len++] = (base64_value(temp[0]) << 2) + ((base64_value(temp[1]) & 0x30) >> 4);
      output[output_len++] = ((base64_value(temp[1]) & 0xf) << 4) + ((base64_value(temp[2]) & 0x3c) >> 2);
      output[output_len++] = ((base64_value(temp[2]) & 0x3) << 6) + base64_value(temp[3]);
      i = 0;
    }
  }

  if (i) {
    if (i == 2) {
      output[output_len++] = (base64_value(temp[0]) << 2) + ((base64_value(temp[1]) & 0x30) >> 4);
    } else if (i == 3) {
      output[output_len++] = (base64_value(temp[0]) << 2) + ((base64_value(temp[1]) & 0x30) >> 4);
      output[output_len++] = ((base64_value(temp[1]) & 0xf) << 4) + ((base64_value(temp[2]) & 0x3c) >> 2);
    }
  }

  return output_len;
}

inline bool isBase64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

inline uint8_t base64_value(char c) {
  const char* pos = strchr(base64_chars, c);
  if (pos) {
    return pos - base64_chars;
  }
  return 0;
}

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

  // Initialize I2S
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX), // Master mode, transmit
    .sample_rate = 16000, // Sample rate
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // 16 bits per sample
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, // Stereo format
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = 0, // Interrupt allocation flag
    .dma_buf_count = 8,
    .dma_buf_len = 64
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCK_IO,
    .ws_io_num = I2S_WS_IO,
    .data_out_num = I2S_DO_IO,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM, &pin_config);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Construct the request payload
    StaticJsonDocument<512> doc;
    doc["input"]["text"] = "Hello, this is a test of TTS with XC3744.";
    doc["voice"]["languageCode"] = "en-US";
    doc["voice"]["name"] = "en-US-Wavenet-D";
    doc["audioConfig"]["audioEncoding"] = "MP3";

    String requestBody;
    serializeJson(doc, requestBody);

    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    // Send POST request
    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Request succeeded, response:");
      Serial.println(response);

      // Extract Base64 audio data
      StaticJsonDocument<1024> responseDoc;
      deserializeJson(responseDoc, response);
      String audioContent = responseDoc["audioContent"];

      // Decode Base64 audio data
      int audioDataLength = (audioContent.length() * 3) / 4;
      uint8_t* audioData = (uint8_t*)malloc(audioDataLength);
      int decodedLength = base64_decode(audioData, audioContent.c_str(), audioContent.length());

      // Play audio through I2S
      size_t bytes_written;
      i2s_write(I2S_NUM, audioData, decodedLength, &bytes_written, portMAX_DELAY);

      free(audioData);
    } else {
      // Print error message if request fails
      Serial.print("Error in request, error code: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Close the connection
  } else {
    // Print message if Wi-Fi is not connected
    Serial.println("WiFi not connected");
  }

  delay(10000); // Wait 10 seconds before making the next request
}
