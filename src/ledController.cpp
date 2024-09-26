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