#include "main.h"

// Helper functions
Difficulty stringToDifficulty(String difficultyStr) {
    if (difficultyStr == "EASY") return Difficulty::EASY;
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

bool isWifiConnected = false;

void setup() {
	Serial.begin(9600);

	loadSettings();
	if (wifiConnect(getSsid(), getPassword())) {
		isWifiConnected = true;
	}

	// Create initial game object with various settings stored
	gameObject.gameState = STARTUP;
	gameObject.difficulty = stringToDifficulty(getDifficulty());
	gameObject.gameMode = GUESS;
	gameObject.colourBlindMode = stringToCBM(getColourBlindMode());

	setupWebServer(getDifficulty(), getColourBlindMode());
	// setupLED();
	// Need to set up the button device
	setupLidSwitch();
}

void loop() {

	switch (gameObject.gameMode) {
		case GUESS:
			Serial.println("Game mode: Guess");
			switch (gameObject.gameState) {
				case INIT:
					Serial.println("Starting game...");
					while (!isLidClosedTemp()) {
						Serial.println("Please close the lid!");
						closeLidLights();
					};

					// Set colour baseline
					setupColourSensor();
					break;
				case STARTUP:
					switch (gameObject.difficulty) {
						case EASY:
							Serial.println("Picking random easy colour");
							setEasyColour();
							break;
						case HARD:
							Serial.println("Picking random hard colour");
							setHardColour();
							break;
					}
					gameObject.gameState = WAITING;
					break;
				case WAITING:
					Serial.println("Waiting for user to find item");
					while (isLidClosedTemp()) {
						delay(1000);
					}
					while (!isLidClosedTemp()) {
						delay(1000);
					}
					gameObject.gameState = SCANNING;
					break;
				case SCANNING:
					Serial.println("Processing item");
					std::tuple<int, int, int> scannedColour = getColour();
					int accuracy = compareColour(std::get<0>(scannedColour), std::get<1>(scannedColour), std::get<2>(scannedColour));
					Serial.print("Accuracy is ");
					Serial.println(accuracy);

					Serial.println("Open lid to restart game");
					while (isLidClosedTemp()) {
						delay(1000);
					}
					while (!isLidClosedTemp()) {
						delay(1000);
					}
					gameObject.gameState = STARTUP;
					break;
			}
		case COLLECTION:
			Serial.println("Collection game mode");
		}

}