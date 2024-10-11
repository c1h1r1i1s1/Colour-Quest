#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <SPIFFS.h>
#include "fileAccess.h"
#include "api-key.h"

void createImages(String prompt);