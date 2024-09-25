#include <WiFi.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "fileAccess.h"

void setupWebServer(String difficulty, String colourBlindMode);