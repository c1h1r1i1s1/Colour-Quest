#include <Adafruit_NeoPixel.h>
#include "pinout.h"
#include "fileAccess.h"

void setEasyColour();

void setHardColour();

void displayDynamicStandby();

void setupLED();

int compareColour(int r, int g, int b);

void closeLidLights();

void waitingGlow();
bool addColour(int r, int g, int b);
void resetColourFinder();
String getFoundColours();

void changeGameMode();