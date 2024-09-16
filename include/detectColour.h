#include <Arduino.h>
#include "esp32-hal-timer.h"
#include <tuple>
#include "pinout.h"

void setupColourSensor();

std::tuple<int, int, int> getColour();