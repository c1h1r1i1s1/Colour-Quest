#include <Arduino.h>
#include "main.h"
#include "detectColour.h"

void setup() {
	Serial.begin(9600);

	setupColourSensor();
}

void loop() {
	delay(1000);
	std::tuple<int, int, int> colour = getColour();

	Serial.print(std::get<0>(colour));
	Serial.print(" ");
	Serial.print(std::get<1>(colour));
	Serial.print(" ");
	Serial.println(std::get<2>(colour));
}