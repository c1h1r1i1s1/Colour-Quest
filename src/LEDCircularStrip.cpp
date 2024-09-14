#include <Adafruit_NeoPixel.h>

// NeoPixel configuration
#define PIN 13
#define NUMPIXELS 24
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Reference color (default: white)
int refR = 255, refG = 255, refB = 255;

// Flag for color comparison mode
bool needToCompare = true;

// Set reference color
void setReferenceColor(int r, int g, int b) {
  refR = r;
  refG = g;
  refB = b;
}

// Calculate similarity percentage between reference and input color
int calculateSimilarity(int r, int g, int b) {
  int diffR = abs(refR - r);
  int diffG = abs(refG - g);
  int diffB = abs(refB - b);
  int maxDiff = 255 * 3; // Maximum possible difference
  int actualDiff = diffR + diffG + diffB;
  float similarity = 100.0 - ((actualDiff * 100.0) / maxDiff);
  return static_cast<int>(similarity); // Return as integer
}

// Get color based on similarity percentage
uint32_t getColorBasedOnAccuracy(int accuracy) {
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

// Display color based on accuracy
void displayBasedOnAccuracy(int accuracy) {
  uint32_t color = getColorBasedOnAccuracy(accuracy);
  for (int i = 0; i < NUMPIXELS; i++) pixels.setPixelColor(i, color);
  pixels.show();
}

// Display dynamic color-moving effect
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

void setup() {
  pixels.begin(); // Initialize NeoPixel
  setReferenceColor(255, 255, 255); // Set reference color
}

void loop() {
  if (needToCompare) {
    int inputR = 25, inputG = 25, inputB = 25; // Example input color
    int accuracy = calculateSimilarity(inputR, inputG, inputB);
    displayBasedOnAccuracy(accuracy);
    delay(5000); // Display result for 5 seconds
    needToCompare = false; // Transition to standby mode
  } else {
    displayDynamicStandby(); // Display dynamic standby effect
  }
}

// Trigger color comparison mode with new input
void activateComparisonMode(int r, int g, int b) {
  setReferenceColor(r, g, b); // Set new reference color
  needToCompare = true;      // Enable comparison mode
}
