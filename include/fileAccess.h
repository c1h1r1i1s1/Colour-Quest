#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <time.h>

String getDifficulty();
String getColourBlindMode();
String getSsid();
String getPassword();

void loadSettings();
bool saveSettings(String newDifficulty, String newColourBlindMode, String newSsid, String newPassword);