#include "lidSwitch.h"

void setupLidSwitch() {
	pinMode(SWI_PIN, INPUT);
}

// Need to write section on lid reading
bool isLidClosedTemp() {
	int num = round(random(0, 2));
	Serial.println(num);
	return num;
}

bool doubleCheck(bool previous) {
	delay(50);
	if (digitalRead(SWI_PIN) == previous) {
		return previous;
	} else {
		return doubleCheck(digitalRead(SWI_PIN));
	}
}

bool isLidClosed() {
	return doubleCheck(digitalRead(SWI_PIN));
}

bool quickCheck() {
	// return digitalRead(SWI_PIN);
	int num = round(random(0, 2));
	Serial.println(num);
	return num;
}