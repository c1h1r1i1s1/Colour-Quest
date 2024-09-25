#include <SPIFFS.h>
#include <ArduinoJson.h>

String getDifficulty();
String getColourBlindMode();
void loadSettings();
bool saveSettings(String newDifficulty, String newColourBlindMode);