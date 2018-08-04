#include "Particle.h"

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler;

int bsdLcgRand();
int rseed = 1;

void setup() {
	Serial.begin();
	Serial1.begin(230400);

	rseed = 1;
}

void loop() {
	int c = Serial1.read();
	if (c == 0) {
		Log.info("resetting sequence");
		rseed = 1;

		unsigned long startTime = millis();
		while(millis() - startTime < 5000) {
			Serial1.read();
		}
	}

	// Keep the send FIFO as full as possible without blocking
	while(Serial1.availableForWrite() > 2) {
		uint8_t c = (uint8_t) (bsdLcgRand() & 0xff);
		Serial1.write(c);
	}
}

// We could just use rand/srand here, but by using a known algorithm this makes it
// easier to do testing between different systems (PC -> Photon/Electron, etc.)
#define RAND_MAX ((1U << 31) - 1)

// https://rosettacode.org/wiki/Linear_congruential_generator#C
int bsdLcgRand() {
	return rseed = (rseed * 1103515245 + 12345) & RAND_MAX;
}
