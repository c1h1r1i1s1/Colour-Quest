#include <WiFi.h>
#include <HTTPClient.h>
#include <driver/i2s.h>

#define I2S_WS       25 // Word Select pin
#define I2S_BCK      26 // Bit Clock pin
#define I2S_DATA     22 // Data pin

const char* ssid = "your_SSID"; // Replace with your WiFi SSID
const char* password = "your_PASSWORD"; // Replace with your WiFi Password

// URL of the WAV file
const char* audio_url = "http://example.com/sample.wav";

// I2S Configuration
void i2s_init() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 16000, // Sample rate for WAV file
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true // Auto clear descriptor
    };
    
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_DATA,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    // Install and start I2S driver
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_set_clk(I2S_NUM_0, 16000, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
}

void setup() {
    Serial.begin(115200);
    
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    
    // Initialize I2S
    i2s_init();
}

void loop() {
    // Download and play WAV audio
    HTTPClient http;
    http.begin(audio_url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        WiFiClient* stream = http.getStreamPtr();
        
        // Buffer for reading data
        uint8_t buffer[1024];
        while (stream->connected() && (stream->available() > 0)) {
            int bytes_read = stream->readBytes(buffer, sizeof(buffer));
            size_t bytes_written;
            i2s_write(I2S_NUM_0, buffer, bytes_read, &bytes_written, portMAX_DELAY);
        }
    } else {
        Serial.println("Failed to download audio");
    }

    http.end(); // Close connection
    delay(10000); // Wait for 10 seconds
}

