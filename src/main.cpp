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
	rtc_gpio_deinit(GPIO_NUM_12); // In case of sleep, re enable pin as digital

	loadSettings();
	setupLED();

	if (wifiConnect(getSsid(), getPassword())) {
		isWifiConnected = true;
		initialiseTime();
	}

	// Create initial game object with various settings stored
	gameObject.gameState = INIT;
	gameObject.difficulty = stringToDifficulty(getDifficulty());
	// if (isWifiConnected) {
	// 	gameObject.gameMode = COLLECTION;
	// } else {
	// 	gameObject.gameMode = GUESS;
	// }
	gameObject.gameMode = GUESS;
	gameObject.colourBlindMode = stringToCBM(getColourBlindMode());

	setupWebServer(getDifficulty(), getColourBlindMode());
	setupLED();
	setupLidSwitch();
	setupButton();
}

void sleep() {
	rtc_gpio_pulldown_en(GPIO_NUM_12);
	esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 1);
	Serial.println("Sleeping");
	esp_deep_sleep_start();
}

void loop() {
	int buttonState = checkPress();
	if (buttonState == 1) {
		gameObject.gameState = STARTUP;
	} else if (buttonState == 3) {
		sleep();
	}

	switch (gameObject.gameMode) {
		case GUESS:
			if (buttonState == 2 && isWifiConnected) {
				gameObject.gameMode = COLLECTION;
				changeGameMode();
				resetColourFinder();
				break;
			}

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
						buttonState = checkPress();
						if (buttonState == 1) {
							gameObject.gameState = STARTUP;
							break;
						} else if (buttonState == 2) {
							gameObject.gameMode = COLLECTION;
							changeGameMode();
							resetColourFinder();
							break;
						} else if (buttonState == 3) {
							sleep();
						}
						delay(500);
					}
					// if (buttonState == 1) {
					// 	gameObject.gameState = STARTUP;
					// 	break;
					// } else if (buttonState == 2) {
					// 	gameObject.gameMode = COLLECTION;
					// 	changeGameMode();
					// 	resetColourFinder();
					// 	break;
					// }

					while (!isLidClosed()) {
						buttonState = checkPress();
						if (buttonState == 1) {
							gameObject.gameState = STARTUP;
							break;
						} else if (buttonState == 2) {
							gameObject.gameMode = COLLECTION;
							changeGameMode();
							resetColourFinder();
							break;
						} else if (buttonState == 3) {
							sleep();
						}
						delay(500);
					}
					// if (buttonState == 1) {
					// 	gameObject.gameState = STARTUP;
					// 	break;
					// } else if (buttonState == 2) {
					// 	gameObject.gameMode = COLLECTION;
					// 	changeGameMode();
					// 	resetColourFinder();
					// 	break;
					// }

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
						buttonState = checkPress();
						if (buttonState == 2) {
							gameObject.gameMode = COLLECTION;
							changeGameMode();
							resetColourFinder();
							break;
						} else if (buttonState == 3) {
							sleep();
						}
						delay(500);
					}

					gameObject.gameState = STARTUP;
					break;
			}
			break;
		case COLLECTION:
			if (buttonState == 2) {
				gameObject.gameState = STARTUP;
				gameObject.gameMode = GUESS;
				changeGameMode();
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
					if (buttonState == 1) {
						Serial.println("Colour finding finished");
						gameObject.gameState = PROCESSING;
						break;
					} else if (buttonState == 2) {
						gameObject.gameState = STARTUP;
						gameObject.gameMode = GUESS;
						changeGameMode();
						break;
					} else if (buttonState == 3) {
						sleep();
					}
					break;
				case SCANNING:
					waitingGlow();
					buttonState = checkPress();
					if (buttonState == 1) {
						Serial.println("Colour finding finished");
						gameObject.gameState = PROCESSING;
						break;
					} else if (buttonState == 2) {
						gameObject.gameState = STARTUP;
						gameObject.gameMode = GUESS;
						changeGameMode();
						break;
					} else if (buttonState == 3) {
						sleep();
					}

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
					String foundColours = getFoundColours();
					String recommendations = whatToDraw(foundColours);
					genTTS(recommendations);
					
					gameObject.gameState = STARTUP;
					break;
			}
			break;
		case TEST:
			// rtc_gpio_pulldown_en(GPIO_NUM_12);
			// esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 1);
			// Serial.println("Sleeping");
			// esp_deep_sleep_start();
			break;
	}
}