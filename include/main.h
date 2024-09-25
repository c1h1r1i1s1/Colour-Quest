#include <Arduino.h>
#include "detectColour.h"
#include "webServer.h"
#include "pinout.h"
// #include "LEDCircularStrip.h"

enum GameState {
	STARTUP,
	MEDIUM,
	HIGH
};

enum Difficulty {
	EASY,
	MEDIUM,
	HARD
};

enum ColourBlindMode {
	NONE,
	PROTANOPIA,
	DEUTERANOPIA,
	TRITANOPIA
};

enum GameMode {
	GUESS,
	COLLECTION
};

struct GameObject {
	enum GameState gameState;
	enum Difficulty difficulty;
	enum GameMode gameMode;
	enum ColourBlindMode colourBlindMode;

};