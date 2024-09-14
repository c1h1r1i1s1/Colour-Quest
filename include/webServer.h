#include <WiFi.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

void handleRoot();

void setupWebServer();

void webListener();