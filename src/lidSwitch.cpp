#include "lidSwitch.h"

void setupLidSwitch() {
	pinMode(SWI_PIN, INPUT);
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
	return digitalRead(SWI_PIN);
}