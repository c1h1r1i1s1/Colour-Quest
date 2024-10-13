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

	setupLED();

	loadSettings();
	if (wifiConnect(getSsid(), getPassword())) {
		isWifiConnected = true;
		initialiseTime();
	}
	// Create initial game object with various settings stored
	gameObject.gameState = INIT;
	gameObject.difficulty = stringToDifficulty(getDifficulty());
	if (isWifiConnected) {
		gameObject.gameMode = COLLECTION;
	} else {
		gameObject.gameMode = GUESS;
	}
	// gameObject.gameMode = TEST;
	gameObject.colourBlindMode = stringToCBM(getColourBlindMode());

	setupWebServer(getDifficulty(), getColourBlindMode());
	setupLED();
	// Need to set up the button device if using
	setupLidSwitch();
}

void loop() {

	switch (gameObject.gameMode) {
		case GUESS:
			switch (gameObject.gameState) {
				case INIT:
					Serial.println("Starting guess game...");
					while (!isLidClosed()) {
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
					while (isLidClosed()) {
						delay(500);
					}
					while (!isLidClosed()) {
						delay(500);
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
					while (isLidClosed()) {
						delay(500);
					}

					gameObject.gameState = STARTUP;
					break;
			}
			break;
		case COLLECTION:
			switch (gameObject.gameState) {
				case INIT:
					Serial.println("Starting collection game...");
					while (!isLidClosed()) {
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
						if (!isLidClosed()) {
							gameObject.gameState = SCANNING;
							Serial.println("Waiting for user to close lid");
						}
					}
					break;
				case SCANNING:
					waitingGlow();
					if (quickCheck()) {
						if (isLidClosed()) {
							Serial.println("Scanning item...");
							scannedColour = getColour();
							int r = std::get<0>(scannedColour), g = std::get<1>(scannedColour), b = std::get<2>(scannedColour);
							Serial.println("Adding colour to array");
							if (addColour(r, g, b)) {
								waitingGlow();
								Serial.println("Colour finding finished");
								gameObject.gameState = PROCESSING;
							} else {
								gameObject.gameState = WAITING;
							}
						}
					}
					break;
				case PROCESSING:
					// displayDynamicStandby();
					// generatingStandby();
					
					String foundColours = getFoundColours();
					createImages(foundColours);
					
					gameObject.gameState = STARTUP;
					break;
			}
			break;
		case TEST:
			Serial.println(quickCheck());
			delay(500);

			// createImages("(139,0,0), (154,205,50), (139,69,19)");
			// delay(1000000);
			// gameObject.gameMode = GUESS;
			break;
	}
}