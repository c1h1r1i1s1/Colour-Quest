#include <Adafruit_NeoPixel.h>
#include "pinout.h"

void setReferencecolour(int r, int g, int b);

int calculateSimilarity(int r, int g, int b);

uint32_t getcolourBasedOnAccuracy(int accuracy);

void displayBasedOnAccuracy(int accuracy);

void displayDynamicStandby();

void activateComparisonMode(int r, int g, int b);

void setupLED();

void showLED();
