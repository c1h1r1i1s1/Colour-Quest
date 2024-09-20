#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>

// RFID Module pins
#define SS_PIN 10
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);

// NeoPixel configuration
#define LED_PIN 8
#define NUMPIXELS 24
Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Reference color
int refR = 255, refG = 255, refB = 255;

// Flag for color comparison mode
bool needToCompare = false;

// Set reference color randomly
void setRandomReferenceColor() {
  refR = random(0, 256);
  refG = random(0, 256);
  refB = random(0, 256);
}

// Calculate similarity percentage between reference and input color
int calculateSimilarity(int r, int g, int b) {
  int diffR = abs(refR - r);
  int diffG = abs(refG - g);
  int diffB = abs(refB - b);
  int maxDiff = 255 * 3;
  int actualDiff = diffR + diffG + diffB;
  float similarity = 100.0 - ((actualDiff * 100.0) / maxDiff);
  return static_cast<int>(similarity);
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
    delay(20);
  }
}

void setup() {
  // Initialize Serial, RFID, and NeoPixel
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  pixels.begin();
  
  Serial.println("Waiting for card scan...");
}

void loop() {
  // Standby mode with dynamic lighting
  if (!needToCompare) {
    displayDynamicStandby();
  }

  // Check for RFID card
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.println("Card detected!");

    // Set a random color as the reference after scanning
    setRandomReferenceColor();
    Serial.print("Reference color: R=");
    Serial.print(refR); Serial.print(", G=");
    Serial.print(refG); Serial.print(", B=");
    Serial.println(refB); 

    // Keep lights on with the reference color
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(refR, refG, refB));
    }
    pixels.show();  // Make sure the lights show up

    // Enter comparison mode
    needToCompare = true;
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  // If in comparison mode
  if (needToCompare) {
    delay(5000); // Simulate a delay for input

    // Example input color (Replace with actual sensor input)
    int inputR = random(0, 256), inputG = random(0, 256), inputB = random(0, 256);
    int accuracy = calculateSimilarity(inputR, inputG, inputB);

    // Display result based on accuracy
    displayBasedOnAccuracy(accuracy);
    delay(5000); // Display for 5 seconds

    // Return to standby mode
    needToCompare = false;
  }
}

