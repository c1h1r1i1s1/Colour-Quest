#include "ledController.h"
// #include <Adafruit_NeoPixel.h>

// NeoPixel configuration
#define NUMPIXELS 24

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Reference colour (default: white)
int refR = 255, refG = 255, refB = 255;

void updateRingColours() {
	for(int i = 0; i < NUMPIXELS; i++) {
		pixels.setPixelColor(i, pixels.Color(refR, refG, refB));
	}
	pixels.show();
}

void setEasyColour() {
	refR = random(0, 1)*255;
	refG = random(0, 1)*255;
	refB = random(0, 1)*255;
	if (refR+refG+refB == 0) {
		refR = 255;
	}

	updateRingColours();
}

void setHardColour() {
	refR = random(0, 255);
	refG = random(0, 255);
	refB = random(0, 255);
	if (refR+refG+refB == 0) {
		refR = 255;
	}

	updateRingColours();
}

// Calculate similarity percentage between reference and input colour
int calculateSimilarity(int r, int g, int b) {
	int diffR = abs(refR - r);
	int diffG = abs(refG - g);
	int diffB = abs(refB - b);
	int maxDiff = 255 * 3; // Maximum possible difference
	int actualDiff = diffR + diffG + diffB;
	float similarity = 100.0 - ((actualDiff * 100.0) / maxDiff);
	return static_cast<int>(similarity); // Return as integer
}

// Get colour based on similarity percentage
uint32_t getColourBasedOnAccuracy(int accuracy) {
	if (accuracy > 95) return pixels.Color(0, 100, 0);   // Deep Green
	if (accuracy > 90) return pixels.Color(0, 128, 0);   // Green
	if (accuracy > 85) return pixels.Color(50, 205, 50); // Lime Green
	if (accuracy > 80) return pixels.Color(154, 205, 50);// Yellow Green
	if (accuracy > 75) return pixels.Color(255, 255, 0); // Yellow
	if (accuracy > 70) return pixels.Color(255, 215, 0); // Gold
	if (accuracy > 65) return pixels.Color(255, 165, 0); // Orange
	if (accuracy > 60) return pixels.Color(255, 69, 0);  // Orange Red
	if (accuracy > 50) return pixels.Color(255, 0, 0);   // Red
	return pixels.Color(139, 0, 0); // Dark Red
}

// Display colour based on accuracy
void displayBasedOnAccuracy(int accuracy) {
	uint32_t colour = getColourBasedOnAccuracy(accuracy);
	for (int i = 0; i < NUMPIXELS; i++) pixels.setPixelColor(i, colour);
	pixels.show();
}

// Display dynamic colour-moving effect
void displayDynamicStandby() {
	for (long firstPixelHue = 0; firstPixelHue < 65536; firstPixelHue += 512) {
		for (int i = 0; i < NUMPIXELS; i++) {
			int pixelHue = firstPixelHue + (i * 65536L / NUMPIXELS);
			pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
		}
		pixels.show();
		delay(20); // Adjust speed
	}
}

uint32_t foundColours[4] = {
	pixels.Color(0, 0, 0),
	pixels.Color(0, 0, 0),
	pixels.Color(0, 0, 0),
	pixels.Color(0, 0, 0)
};

int colourSection = 0;
float pulseValue = 0;
bool pulseUp = true;

void resetColourFinder() {
	colourSection = 0;
}

bool addColour(int r, int g, int b) {
	foundColours[colourSection++] = pixels.Color(r, g, b);
	if (colourSection == 4) {
		return 1;
	} else {
		return 0;
	}
}

String getFoundColours() {
	String rgbString = "";
  
	for (int i = 0; i < numColours; i++) {
		uint32_t color = colours[i];

		// Extract red, green, and blue components from the 32-bit color value
		uint8_t r = (color >> 16) & 0xFF;
		uint8_t g = (color >> 8) & 0xFF;
		uint8_t b = color & 0xFF;

		// Append the RGB values to the string
		rgbString += "(" + String(r) + "," + String(g) + "," + String(b) + ")";

		if (i < numColours - 1) {
			rgbString += ", ";
		}
	}

	return rgbString;
}

void waitingGlow() {
	if (pulseUp) {
		pulseValue += 0.01;
		if (pulseValue >= 1.0) {
			pulseValue = 1.0;
			pulseUp = false;
		}
	} else {
		pulseValue -= 0.01;
		if (pulseValue <= 0.0) {
			pulseValue = 0.0;
			pulseUp = true;
		}
	}

	uint8_t brightness = (uint8_t)(pulseValue * 255);

	// Pulse the sections white that aren't filled with colour.
	for (int i = colorSection * (NUMPIXELS / 4); i < NUMPIXELS; i++) {
		pixels.setPixelColor(i, pixels.Color(brightness, brightness, brightness));
	}

	// Fill the found colour sections
	int ledsPerSection = NUMPIXELS / 4;
	for (int i = 0; i < colourSection; i++) {
		for (int j = i * ledsPerSection; j < (i + 1) * ledsPerSection; j++) {
			pixels.setPixelColor(j, colors[i]);
		}
	}

	pixels.show();
}

void closeLidLights() {
	refR = 255;
	refG = 255;
	refB = 255;

	updateRingColours();
	delay(500);

	refR = 0;
	refG = 0;
	refB = 0;

	updateRingColours();
	delay(500);
}

void setupLED() {
	pixels.begin(); // Initialize NeoPixel
	displayDynamicStandby();
}

int compareColour(int r, int g, int b) {
	int accuracy = calculateSimilarity(r, g, b);
	displayBasedOnAccuracy(accuracy);
	return accuracy;
}