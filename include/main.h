#include <Arduino.h>
#include "detectColour.h"
#include "webServer.h"
#include "pinout.h"
#include "fileAccess.h"
#include "wifiCon.h"
#include "ledController.h"
#include "lidSwitch.h"
#include "gptAccess.h"
#include "button.h"

enum GameState {
	INIT,
	STARTUP,
	SCANNING,
	WAITING,
	PROCESSING
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
	COLLECTION,
	TEST
};

struct GameObject {
	enum GameState gameState;
	enum Difficulty difficulty;
	enum GameMode gameMode;
	enum ColourBlindMode colourBlindMode;
};

GameObject gameObject;