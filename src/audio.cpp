#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Base64.h>
#include "SPIFFS.h"

#include "AudioOutputI2S.h"
#include "AudioFileSourceSPIFFS.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"

const char* ssid = "GriGri";
const char* password = "Whippers@129";
const char* api_key = "AIzaSyDusPba9Y8wTdxTqaZL1PJHHL3BHocVsV8";
String url = "https://texttospeech.googleapis.com/v1/text:synthesize?key=" + String(api_key);

AudioGeneratorMP3 *mp3;
AudioFileSourceSPIFFS *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string) {
	(void)cbData;
	Serial.printf("ID3 callback for: %s = '", type);

	if (isUnicode) {
		string += 2;
	}

	while (*string) {
		char a = *(string++);
		if (isUnicode) {
			string++;
		}
		Serial.printf("%c", a);
	}
	Serial.printf("'\n");
	Serial.flush();
}

void printEscapedCharacters(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        switch (str[i]) {
            case '\\':
                Serial.print("\\\\"); // Print the backslash
                break;
            case '\n':
                Serial.print("\\n"); // Print newline as '\n'
                break;
            case '\r':
                Serial.print("\\r"); // Print carriage return as '\r'
                break;
            case '\t':
                Serial.print("\\t"); // Print tab as '\t'
                break;
            default:
                Serial.print(str[i]); // Print regular characters
                break;
        }
    }
    Serial.println(); // Add a new line at the end for better readability
}

void getTTS() {
	File audioFile = SPIFFS.open("/splat.mp3", FILE_WRITE);
	if (!audioFile) {
		Serial.println("Failed to open file for writing");
		return;
	}

	JsonDocument doc;
    doc["input"]["text"] = "Nice colours man! I reckon you should draw a beautiful picnic in a forest, with a red picnic rug, brown basket, and deep green trees";
    doc["voice"]["languageCode"] = "en-US";
    doc["voice"]["name"] = "en-US-Wavenet-D";
    doc["audioConfig"]["audioEncoding"] = "MP3";
    String requestBody;
    serializeJson(doc, requestBody);

    HTTPClient client;
    if (!client.begin(url)) {
		Serial.println("Connection to API failed!");
	}
    client.addHeader("Content-Type", "application/json");
    int httpResponseCode = client.POST(requestBody);

	if (httpResponseCode > 0) {
		WiFiClient* stream = client.getStreamPtr();

		// Store stream buffer chunk
		// Decode each chunk of b64
		// If incoming chunk is not even b64, store remainder in 'overlap'
		// Once decoded, write to file.

		size_t allocatedSize = 0;  // Currently allocated size
		char* audioContent = NULL;  // Pointer to hold audio content
		char overlap[4] = "";
		bool beginning = true;

		while (stream->available()) {
			String chunkStr = stream->readStringUntil('\n');
			int chunkLength = chunkStr.length();
			if (chunkLength > 0) {
				Serial.println(chunkStr[chunkLength-1]);
				if (chunkStr[chunkLength-1] == '\r') {
					chunkStr[chunkLength-1] = '\0';
					chunkLength -= 1;
				}
			}
			if (chunkLength < 6) {
				// Serial.println(chunkStr);
				continue; // If it is this weird header thing.
			}
			// Serial.print(chunkStr);

			if (chunkLength + strlen(overlap) > allocatedSize) { // prefix with previous overlap for decoding
				allocatedSize = chunkLength + strlen(overlap) + 1;  // +1 for null terminator
				
				char* newAudioContent = (char*)realloc(audioContent, allocatedSize);
				if (newAudioContent == NULL) {
					Serial.println("Failed to allocate memory for audio content.");
					return;
				}
				audioContent = newAudioContent;
			}

			Serial.println("Incoming overlap content");
			printEscapedCharacters(overlap);
			Serial.println(chunkLength);
			Serial.println(strlen(overlap));
			Serial.println(allocatedSize);

			memset(audioContent, 0, allocatedSize); // Clear the buffer

			strncpy(audioContent, overlap, 5); // First copy overlap
			Serial.println(audioContent);
			strncat(audioContent, chunkStr.c_str(), allocatedSize-1); // Then copy the new stuff
			audioContent[allocatedSize - 1] = '\0';
			Serial.println(audioContent);

			// Now decode in parts
			// First check for beginning/end of json
			char* startPos = audioContent;
			if (beginning) {
				startPos = strstr(audioContent, "\"audioContent\": \"");
				if (startPos == NULL) {
					Serial.println("Couldn't find beginning");
					continue;
				}
				Serial.println("found the beginning");
				startPos += strlen("\"audioContent\": \"");
				beginning = false;
			}
			char* endPos = strchr(startPos, '\"');
			if (endPos != NULL) {
				// Final iteration
				// Serial.println("end point found");
				size_t encodedAudioLength = endPos - startPos;
				startPos[encodedAudioLength] = '\0';
			} else {
				endPos = startPos + strlen(startPos);
			}

			// Now decode in chunks
			const int chunkSize = 512;  // Base64 standard chunk size
			char base64Chunk[chunkSize + 1]; // Buffer for Base64 chunk
			int totalDecodedLength = 0;

			// Process the Base64 data in chunks
			Serial.println("entering chunk loop");
			while (startPos < endPos) {
				int remaining = endPos - startPos;
				Serial.println(remaining);
				if (remaining < 4) {
					Serial.println("next");
					break;
				} else if (remaining < 512) {
					int remainder = remaining % 4;
					if (remainder > 0) {
						Serial.println("new overlap created");
						strcpy(overlap, startPos + remaining - remainder);
						remaining -= remainder;
					}
				}

				int bytesToCopy = remaining < chunkSize ? remaining : chunkSize;

				// Copy the next chunk of Base64 data
				strncpy(base64Chunk, startPos, bytesToCopy);
				base64Chunk[bytesToCopy] = '\0'; // Null terminate the chunk

				// Decode the current chunk
				int decodedLength = Base64.decodedLength(base64Chunk, bytesToCopy);
				char* decodedAudioChunk = (char*)malloc(decodedLength + 1);
				if (decodedAudioChunk != NULL) {
					if (Base64.decode(decodedAudioChunk, base64Chunk, bytesToCopy) == 0) {
						free(decodedAudioChunk);
						free(audioContent);
						Serial.println("Issue decoding");
						return;
					}

					decodedAudioChunk[decodedLength] = '\0';

					audioFile.write((const uint8_t*)decodedAudioChunk, decodedLength);

					free(decodedAudioChunk);
				} else {
					Serial.println("Issue with creating audio chunk malloc");
					free(audioContent);
					return;
				}
				startPos += bytesToCopy;
			}

		}

		Serial.println("Gathered the mp3");
		audioFile.close();
		free(audioContent);

		audioLogger = &Serial;
		file = new AudioFileSourceSPIFFS("/splat.mp3");
		id3 = new AudioFileSourceID3(file);
		id3->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");
		out = new AudioOutputI2S(0,AudioOutputI2S::INTERNAL_DAC);
		mp3 = new AudioGeneratorMP3();
		mp3->begin(id3, out);
	} else {
		Serial.print("Error in request, error code: ");
		Serial.println(httpResponseCode);
		return;
	}
}

void setup() {
	Serial.begin(9600);
	if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.println("Connecting to WiFi...");
	}
	Serial.println("Connected to WiFi!");

	getTTS();
}

void loop() {
	if (mp3->isRunning()) {
		if (!mp3->loop()) mp3->stop();
	} else {
		Serial.printf("MP3 done\n");
		delay(1000);
	}
}