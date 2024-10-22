#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Base64.h>
#include "SPIFFS.h"

#include "AudioOutputI2S.h"
#include "AudioFileSourceSPIFFS.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"

#include "api-key.h"

void genTTS(String text);