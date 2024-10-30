#include <Arduino.h>
#include "esp32-hal-timer.h"
#include <tuple>
#include "pinout.h"

void setupColourSensor();
void resetColourSensor();

std::tuple<int, int, int> getColour();