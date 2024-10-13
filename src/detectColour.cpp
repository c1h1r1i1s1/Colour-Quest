#include "detectColour.h"

int colourFrequencyCount = 0; // count the frequency
int rgbArray[3]; // store the RGB value
int colourScanIndex = 0; // filter of RGB queue
float initcolourScale[3]; // save the RGB Scale factor

hw_timer_t *timer = NULL;  // Hardware timer

// Initialise pins for colour sensor
void TSC_Init() {
	pinMode(S0, OUTPUT);
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);
	pinMode(OUT, INPUT);
	digitalWrite(S0, LOW); // OUTPUT FREQUENCY SCALING 2%
	digitalWrite(S1, HIGH);
}

// Function to tell board which colour to read
void TSC_Filtercolour(int Level01, int Level02) {
	if(Level01 != 0)
		Level01 = HIGH;
	if(Level02 != 0)
		Level02 = HIGH;
	digitalWrite(S2, Level01);
	digitalWrite(S3, Level02);
}

// Function to count amount of a colour
void IRAM_ATTR TSC_Count() {
	colourFrequencyCount++;
}

// Function to select which colour to read
void scanColour(int Level0, int Level1) {
	colourFrequencyCount = 0;
	colourScanIndex++;
	TSC_Filtercolour(Level0, Level1);
	// Scan diode for 0.1 seconds
	timerAlarmWrite(timer, 200000, true);
	timerAlarmEnable(timer);
}

// Go through RGB, scanning Red, Green, Blue individually
void IRAM_ATTR TSC_Callback() {
	switch(colourScanIndex) {
		case 0:
			scanColour(LOW, LOW); // Start red scan
			break;
		case 1:
			rgbArray[0] = colourFrequencyCount; // Store red
			scanColour(HIGH, HIGH); // Start Green scan
			break;
		case 2:
			rgbArray[1] = colourFrequencyCount; // Store green
			scanColour(LOW, HIGH); // Start blue scan
			break;
		case 3:
			rgbArray[2] = colourFrequencyCount; // Store blue
			scanColour(HIGH, LOW); // Clear filter
			break;
		default:
			colourFrequencyCount = 0;
			break;
	}
}


void setupColourSensor() {
	pinMode(OE, OUTPUT);
	pinMode(LED, OUTPUT);
	digitalWrite(OE, LOW);
	digitalWrite(LED, LOW);

	TSC_Init();

	// Initialize hardware timer (timer 0, 80 prescaler for 1us per tick)
	timer = timerBegin(0, 80, true);
	timerAttachInterrupt(timer, &TSC_Callback, true);

	// Every time the device sends readings to OUT, trigger TSC_Count to collect reading.
	attachInterrupt(digitalPinToInterrupt(OUT), TSC_Count, RISING);

	// Set timer for 1 second (1000000 microseconds)
	timerAlarmWrite(timer, 100000, true);
	timerAlarmEnable(timer);

	delay(1000);

	initcolourScale[0] = 255.0 / rgbArray[0]; // R Scale factor
	initcolourScale[1] = 255.0 / rgbArray[1]; // G Scale factor
	initcolourScale[2] = 255.0 / rgbArray[2]; // B Scale factor
}

int normaliseColour(float value, float max) {
	return int(sin(3*pow(value/max, 3)/2)*255.0);
}

std::tuple<int, int, int> getColour() {

	int colourArray[3][3];

	int consistentCount = -1;
	while (consistentCount < 2) {
		colourScanIndex = 0;
		float temp[3] = {0, 0, 0};
		float max = rgbArray[0] * initcolourScale[0];

		// Get max value for scaling
		for (int i = 0; i < 3; i++) {
			temp[i] = rgbArray[i] * initcolourScale[i];
			if (temp[i] > max) {
				max = temp[i];
			}
		}

		// get averages
		if (consistentCount == -1) {
			colourArray[++consistentCount][0] = normaliseColour(temp[0], max);
			colourArray[consistentCount][1] = normaliseColour(temp[1], max);
			colourArray[consistentCount][2] = normaliseColour(temp[2], max);
		} else {
			colourArray[++consistentCount][0] = normaliseColour(temp[0], max);
			colourArray[consistentCount][1] = normaliseColour(temp[1], max);
			colourArray[consistentCount][2] = normaliseColour(temp[2], max);
			for (int i=0; i<3; i++) {
				// If the difference is too large, restart
				if (pow(colourArray[consistentCount][i]-colourArray[consistentCount-1][i], 2) > 300) {
					colourArray[0][0] = colourArray[consistentCount][0];
					colourArray[0][1] = colourArray[consistentCount][1];
					colourArray[0][2] = colourArray[consistentCount][2];
					consistentCount = -1;
					break;
				}
			}
		}

		delay(650);
	}

	int r = 0;
	int g = 0;
	int b = 0;

	for (int i=0; i<3; i++) {
		r += colourArray[i][0];
		g += colourArray[i][1];
		b += colourArray[i][2];
	}

	r = r/4;
	g = g/4;
	b = b/4;


	return std::make_tuple(r, g, b);
}