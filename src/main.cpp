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
int buttonStateMain = 0;

void setup() {
	Serial.begin(9600);

	loadSettings();
	Serial.println("Loaded Settings");
	setupLED();
	Serial.println("Setup LED");

	if (wifiConnect(getSsid(), getPassword())) {
		isWifiConnected = true;
	}

	// Create initial game object with various settings stored
	gameObject.gameState = INIT;
	gameObject.difficulty = stringToDifficulty(getDifficulty());
	gameObject.gameMode = GUESS;
	gameObject.colourBlindMode = stringToCBM(getColourBlindMode());

	setupWebServer(getDifficulty(), getColourBlindMode());
	setupLidSwitch();
	setupButton();
}

void sleep() {
	rtc_gpio_pulldown_en(GPIO_NUM_4);
	esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0);
	changeGameMode();
	turnOffLEDs();
	Serial.println("Sleeping");
	delay(200);
	esp_deep_sleep_start();
}

void loop() {

	switch (gameObject.gameMode) {
		case GUESS:
			switch (gameObject.gameState) {
				case INIT:
					Serial.println("Starting guess game...");
					if (isLidClosed()) {
						while (isLidClosed()) {
							Serial.println("Please clear mouth!");
							closeLidLights();
						};
					}
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
					buttonStateMain = checkPress();
					while (isLidClosed()) {
						if (buttonStateMain == 1) {
							gameObject.gameState = STARTUP;
							break;
						} else if (buttonStateMain == 2) {
							gameObject.gameMode = COLLECTION;
							gameObject.gameState = STARTUP;
							changeGameMode();
							resetColourFinder();
							break;
						} else if (buttonStateMain == 3) {
							sleep();
						}
						buttonStateMain = checkPress();
					}
					if (buttonStateMain > 0) {
						break;
					}

					while (!isLidClosed()) {
						buttonStateMain = checkPress();
						if (buttonStateMain == 1) {
							gameObject.gameState = STARTUP;
							break;
						} else if (buttonStateMain == 2) {
							gameObject.gameMode = COLLECTION;
							gameObject.gameState = STARTUP;
							changeGameMode();
							resetColourFinder();
							break;
						} else if (buttonStateMain == 3) {
							sleep();
						}
					}
					if (buttonStateMain > 0) {
						break;
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

					// Serial.println("Open lid to restart game");
					unsigned long startTime = millis();
					while (isLidClosed()) {
						if (millis() - startTime >= 3000) {
							gameObject.gameState = WAITING;
							break;
						}
						buttonStateMain = checkPress();
						if (buttonStateMain == 1) {
							gameObject.gameState = STARTUP;
							break;
						} else if (buttonStateMain == 2) {
							gameObject.gameMode = COLLECTION;
							gameObject.gameState = STARTUP;
							changeGameMode();
							resetColourFinder();
							break;
						} else if (buttonStateMain == 3) {
							sleep();
						}
					}
					if (buttonStateMain > 0) {
						break;
					}

					gameObject.gameState = WAITING;
					break;
			}
			break;
		case COLLECTION:
			if (!isWifiConnected) {
				gameObject.gameMode = GUESS;
				error();
				break;
			}
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
					buttonStateMain = checkPress();
					if (buttonStateMain == 1) {
						Serial.println("Colour finding finished");
						gameObject.gameState = PROCESSING;
						break;
					} else if (buttonStateMain == 2) {
						gameObject.gameState = STARTUP;
						gameObject.gameMode = GUESS;
						changeGameMode();
						break;
					} else if (buttonStateMain == 3) {
						sleep();
					}
					break;
				case SCANNING:
					waitingGlow();
					buttonStateMain = checkPress();
					if (buttonStateMain == 1) {
						Serial.println("Colour finding finished");
						gameObject.gameState = PROCESSING;
						break;
					} else if (buttonStateMain == 2) {
						gameObject.gameState = STARTUP;
						gameObject.gameMode = GUESS;
						changeGameMode();
						break;
					} else if (buttonStateMain == 3) {
						sleep();
					}

					if (quickCheck()) {
						if (isLidClosed()) {
							Serial.println("Scanning item...");
							scannedColour = getColour();
							int r = std::get<0>(scannedColour), g = std::get<1>(scannedColour), b = std::get<2>(scannedColour);
							Serial.print(r);
							Serial.print(" ");
							Serial.print(g);
							Serial.print(" ");
							Serial.println(b);
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
					// Need white loading animation
					delay(500);
					loading();
					String foundColours = getFoundColours();
					Serial.println(foundColours);
					String recommendations = whatToDraw(foundColours);
					Serial.println(recommendations);
					genTTS(recommendations);
					
					gameObject.gameState = STARTUP;
					resetColourSensor();
					break;
			}
			break;
		case TEST:
			Serial.println(isLidClosed());
			setHardColour();
			delay(5000);
			break;
	}
}