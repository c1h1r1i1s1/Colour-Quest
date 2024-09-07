#include <Adafruit_NeoPixel.h>

// Define what pin the NeoPixel is attached to and how many LEDs there are
#define PIN 17
#define NUMPIXELS 24

// Create the NeoPixel object
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Define the score
int score = 0; // Change this value from 0 to 100

// Define the speed of the LED update in milliseconds
const unsigned long updateInterval = 1000;
unsigned long lastUpdateTime = 0;

void setup() {
  pixels.begin(); // Start the NeoPixel object
}

void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastUpdateTime >= updateInterval) {
    lastUpdateTime = currentTime;

    // Calculate the number of LEDs to light up based on the score
    int numLitPixels = map(score, 0, 100, 0, NUMPIXELS);

    // Set all pixels to red for the total LEDs minus the score
    for (int i = 0; i < NUMPIXELS; i++) {
      if (i < numLitPixels) {
        pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Green color for correct
      } else {
        pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // Red color for incorrect
      }
    }
    
    pixels.show(); // Update the strip with the new contents

    // Update score for demonstration (optional)
    score = (score + 1) % 101; // Increment score and wrap around after 100
  }
}
