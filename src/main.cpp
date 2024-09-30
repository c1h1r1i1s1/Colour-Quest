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
std::tuple<int, int, int> scannedColour;

void setup() {
	Serial.begin(9600);

	loadSettings();
	if (wifiConnect(getSsid(), getPassword())) {
		isWifiConnected = true;
	}

	// Create initial game object with various settings stored
	gameObject.gameState = INIT;
	gameObject.difficulty = stringToDifficulty(getDifficulty());
	gameObject.gameMode = COLLECTION;
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
					gameObject.gameState = STARTUP;
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
					scannedColour = getColour();
					gameObject.gameState = PROCESSING;
					break;
				case PROCESSING:
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
			switch (gameObject.gameState) {
				case INIT:
					Serial.println("Starting game...");
					while (!isLidClosedTemp()) {
						Serial.println("Please close the lid!");
						closeLidLights();
					};

					// Set colour baseline
					setupColourSensor();
					gameObject.gameState = STARTUP;
					break;
				case STARTUP:
					resetColourFinder();
					gameObject.gameState = WAITING;
					Serial.println("Waiting for user to open lid.");
					break;
				case WAITING:
					waitingGlow();
					// Check for open
					if (!quickCheck()) {
						if (!isLidClosedTemp()) {
							gameObject.gameState = SCANNING;
							Serial.println("Waiting for user to close lid");
						}
					}
					break;
				case SCANNING:
					waitingGlow();
					if (quickCheck()) {
						if (isLidClosedTemp()) {
							Serial.println("Scanning item...");
							scannedColour = getColour();
							int r = std::get<0>(scannedColour), g = std::get<1>(scannedColour), b = std::get<2>(scannedColour);
							Serial.println("Adding colour to array");
							if (addColour(r, g, b)) {
								Serial.println("Colour finding finished");
								gameObject.gameState = PROCESSING;
							} else {
								gameObject.gameState = WAITING;
							}
						}
					}
					break;
				case PROCESSING:
					String foundColours = getFoundColours();
					genOutlines(foundColours);
					// Need to do the image saving stuff

					displayDynamicStandby();
					gameObject.gameState = STARTUP;
					break;
			}
		}

}