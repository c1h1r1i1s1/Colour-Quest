#include <Arduino.h>
#include "esp32-hal-timer.h"
#include <tuple>
#include "main.h"

void TSC_Init();
void TSC_FilterColor(int Level01, int Level02);
void IRAM_ATTR TSC_Count();
void scanColour(int Level0, int Level1);
void IRAM_ATTR TSC_Callback();

void setupColourSensor();

int normaliseColour(float value, float max);

std::tuple<int, int, int> getColour();