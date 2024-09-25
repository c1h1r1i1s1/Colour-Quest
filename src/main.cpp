#include "main.h"

// Helper functions
Difficulty stringToDifficulty(String difficultyStr) {
    if (difficultyStr == "EASY") return Difficulty::EASY;
    else if (difficultyStr == "MEDIUM") return Difficulty::MEDIUM;
    else if (difficultyStr == "HARD") return Difficulty::HARD;
    else return Difficulty::EASY;
}

ColourBlindMode stringToCBM(String modeStr) {
    if (modeStr == "NONE") return ColourBlindMode::NONE;
    else if (modeStr == "PROTANOPIA") return ColourBlindMode::PROTANOPIA;
    else if (modeStr == "DEUTERANOPIA") return ColourBlindMode::DEUTERANOPIA;
    else if (modeStr == "TRITANOPIA") return ColourBlindMode::TRITANOPIA;
    else return ColourBlindMode::NONE;
}


void setup() {
	Serial.begin(9600);

	loadSettings();
	Serial.println(getDifficulty());

	GameObject gameObject;
	gameObject.gameState = STARTUP;
	gameObject.difficulty = stringToDifficulty(getDifficulty());
	gameObject.gameMode = GUESS;
	gameObject.colourBlindMode = stringToCBM(getColourBlindMode());

	setupWebServer(getDifficulty(), getColourBlindMode());

	// setupColourSensor();
	
	// setupLED();

	
}

void loop() {
	delay(5000);

	// showLED();

	// Colour scanning example
	// std::tuple<int, int, int> colour = getColour();
	// Serial.print(std::get<0>(colour));
	// Serial.print(" ");
	// Serial.print(std::get<1>(colour));
	// Serial.print(" ");
	// Serial.println(std::get<2>(colour));
}