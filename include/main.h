#include <Arduino.h>
#include "detectColour.h"
#include "webServer.h"
#include "pinout.h"
#include "fileAccess.h"
#include "wifiCon.h"
#include "ledController.h"

enum GameState {
	INIT,
	STARTUP,
	SCANNING,
	WAITING
};

enum Difficulty {
	EASY,
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

GameObject gameObject;