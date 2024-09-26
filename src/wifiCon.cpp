#include "wifiCon.h"

// const char* local_ssid = "GriGri";
// const char* local_password = "Whippers@129";

bool wifiConnect(String local_ssid, String local_password) {
	WiFi.begin(local_ssid.c_str(), local_password.c_str());
	Serial.print("Connecting to WiFi network...");

	int count = 0;
	while (WiFi.status() != WL_CONNECTED) {
		if (count > 10) {
			return 0;
		}
		delay(1000);
		count++;
	}

	return 1;
}