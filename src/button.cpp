#include "button.h"

#define DEBOUNCE_TIME 25
#define LONG_PRESS_TIME 2000
#define DOUBLE_PRESS_TIME 500

Bounce debouncer = Bounce();

unsigned long buttonDownTime = 0;
unsigned long buttonUpTime = 0;
unsigned long lastPressTime = 0;

bool buttonState = LOW;
bool singlePressDetected = false;
bool doublePressDetected = false;
bool longPressDetected = false;

void setupButton() {
	pinMode(BUTTON_PIN, INPUT);
	debouncer.attach(BUTTON_PIN);
	debouncer.interval(DEBOUNCE_TIME);
}

int checkPress() {
	debouncer.update();
	buttonState = debouncer.read();
	int result = 0;

	// Button pressed
	if (buttonState == HIGH) {
		if (buttonDownTime == 0) {
			buttonDownTime = millis();  // Record when the button was pressed
		}

		// Check for long press
		if (millis() - buttonDownTime > LONG_PRESS_TIME && !longPressDetected) {
			result = 3;
			longPressDetected = true;
		}
	}

	// Button released
	if (buttonState == LOW && buttonDownTime != 0) {
		buttonUpTime = millis();

		// If it's a quick press
		if (millis() - buttonDownTime < LONG_PRESS_TIME) {
			// Check if it's a double press
			if (buttonUpTime - lastPressTime < DOUBLE_PRESS_TIME && !doublePressDetected) {
				result = 2;
				doublePressDetected = true;
				singlePressDetected = false;
			} else if (!singlePressDetected) {
				// If no double press is detected, mark a single press (to be handled later)
				singlePressDetected = true;
			}

			lastPressTime = buttonUpTime;
		}

		// Reset the long press detection
		buttonDownTime = 0;
		longPressDetected = false;
	}

	// Handle single press after the double press timeout
	if (singlePressDetected && millis() - lastPressTime > DOUBLE_PRESS_TIME) {
		result = 1;
		singlePressDetected = false;  // Reset the flag after handling
	}

	// Reset double press after handling
	if (doublePressDetected && millis() - lastPressTime > DOUBLE_PRESS_TIME) {
		doublePressDetected = false;  // Reset the double press flag after timeout
	}

	return result;
}