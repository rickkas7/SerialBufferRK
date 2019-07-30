#include "Particle.h"

#include "SerialBufferRK.h"

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler;

// Allocate a serial buffer of 256 bytes attached to Serial1
// call serBuf.read() instead of Serial1.read()
SerialBuffer<4096> serBuf(Serial1);

unsigned long lastDisplay = 0;
int totalReceived = 0;

void setup() {
	Serial.begin();
	Serial1.begin(230400);

	// You must call serBuf.setup() from setup!
	serBuf.setup();
}

void loop() {
	while(true) {
		int c = serBuf.read();
		if (c < 0) {
			break;
		}

		totalReceived++;
	}

	if (millis() - lastDisplay >= 5000) {
		lastDisplay = millis();

		Log.info("totalReceived=%u", totalReceived);
	}
}



