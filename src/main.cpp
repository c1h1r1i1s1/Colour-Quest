#include <Arduino.h>
#include "esp32-hal-timer.h"

#define S0 25
#define S1 26
#define S2 0
#define S3 14
#define OUT 13
#define OE 17
#define LED 16

int colourFrequencyCount = 0; // count the frequency
int rgbArray[3]; // store the RGB value
int colourScanIndex = 0; // filter of RGB queue
float initColorScale[3]; // save the RGB Scale factor

hw_timer_t *timer = NULL;  // Hardware timer

// Init TSC230 and setting Frequency.
void TSC_Init() {
	pinMode(S0, OUTPUT);
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);
	pinMode(OUT, INPUT);
	digitalWrite(S0, LOW); // OUTPUT FREQUENCY SCALING 2%
	digitalWrite(S1, HIGH);
}

// Select the filter color
void TSC_FilterColor(int Level01, int Level02) {
	if(Level01 != 0)
		Level01 = HIGH;
	if(Level02 != 0)
		Level02 = HIGH;
	digitalWrite(S2, Level01);
	digitalWrite(S3, Level02);
}

void IRAM_ATTR TSC_Count() {
	colourFrequencyCount++;
}

void TSC_WB(int Level0, int Level1) { // White balance
	colourFrequencyCount = 0;
	colourScanIndex++;
	TSC_FilterColor(Level0, Level1);
	// Scan diode for 0.1 seconds
	timerAlarmWrite(timer, 100000, true);
	timerAlarmEnable(timer);
}

// Go through, scanning Red, Green
void IRAM_ATTR TSC_Callback() {
	switch(colourScanIndex) {
		case 0:
			TSC_WB(LOW, LOW); // Start red scan
			break;
		case 1:
			rgbArray[0] = colourFrequencyCount; // Store red
			TSC_WB(HIGH, HIGH); // Start Green scan
			break;
		case 2:
			rgbArray[1] = colourFrequencyCount; // Store green
			TSC_WB(LOW, HIGH); // Start blue scan
			break;
		case 3:
			rgbArray[2] = colourFrequencyCount; // Store blue
			TSC_WB(HIGH, LOW); // Clear filter
			break;
		default:
			colourFrequencyCount = 0;
			break;
	}
}
void setup() {
	pinMode(OE, OUTPUT);
	pinMode(LED, OUTPUT);
	digitalWrite(OE, LOW);
	digitalWrite(LED, LOW);

	TSC_Init();
	Serial.begin(9600);

	// Initialize hardware timer (timer 0, 80 prescaler for 1us per tick)
	timer = timerBegin(0, 80, true);
	timerAttachInterrupt(timer, &TSC_Callback, true);

	// Every time the device sends readings to OUT, trigger TSC_Count to collect reading.
	attachInterrupt(digitalPinToInterrupt(OUT), TSC_Count, RISING);

	// Set timer for 1 second (1000000 microseconds)
	timerAlarmWrite(timer, 100, true);
	timerAlarmEnable(timer);

	delay(1000);

	initColorScale[0] = 255.0 / rgbArray[0]; // R Scale factor
	initColorScale[1] = 255.0 / rgbArray[1]; // G Scale factor
	initColorScale[2] = 255.0 / rgbArray[2]; // B Scale factor
}

void loop() {
	colourScanIndex = 0;
	float temp[3];
	float max = rgbArray[0] * initColorScale[0];
	for (int i = 0; i < 3; i++) {
		temp[i] = rgbArray[i] * initColorScale[i];
		if (temp[i] > max) {
			max = temp[i];
		}
	}

	for (int i = 0; i < 3; i++) {
		Serial.print(int(sin(3*pow(temp[i]/max, 3)/2)*255.0));
		Serial.print(" ");
	}
	Serial.println();
	delay(350);
}