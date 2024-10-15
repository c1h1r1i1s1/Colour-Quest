#include "button.h"

volatile unsigned long lastPressTime = 0;
volatile bool doublePressDetected = false;
volatile bool singlePressDetected = false;

void IRAM_ATTR handleButtonPress() {
	unsigned long currentTime = millis();
	singlePressDetected = true;

	if (currentTime - lastPressTime > 100) {
		if (currentTime - lastPressTime < 500) {//} && !doublePressDetected) {
			doublePressDetected = true;
		} else {
			lastPressTime = currentTime;
			doublePressDetected = false;
		}
	}
}

void setupButton() {
	pinMode(BT_PIN, INPUT);
	attachInterrupt(BT_PIN, handleButtonPress, RISING);
}

int checkPress() {
	if (doublePressDetected) {
		doublePressDetected = false;
		singlePressDetected = false;
		return 2;
	} else if (singlePressDetected) {
		unsigned long currentTime = millis();
		if (currentTime - lastPressTime > 500) {
			singlePressDetected = false;
			return 1;
		} else {
			delay(500);
			if (doublePressDetected) {
				doublePressDetected = false;
				singlePressDetected = false;
				return 2;
			} else {
				singlePressDetected = false;
				return 1;
			}
		}
	} else {
		return 0;
	}
}