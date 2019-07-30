#include "Particle.h"

#include "SerialBufferRK.h"

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

SerialLogHandler logHandler;

SerialBuffer<4096> serBuf(Serial1);

void resetSequence();

unsigned long lastDisplay = 0;

size_t numReceived = 0;
size_t maxInBuffer = 0;
size_t totalReceived = 0;

int bsdLcgRand();
int rseed = 1;

void setup() {
	Serial.begin();
	Serial1.begin(230400);

	// You must call serBuf.setup() from setup!
	serBuf.setup();

	Particle.connect();

	resetSequence();
}

void loop() {
	size_t numRead = 0;
	while(true) {
		int c = serBuf.read();
		if (c < 0) {
			break;
		}

		uint8_t expected = (uint8_t) (bsdLcgRand() & 0xff);

		numRead++;
		numReceived++;
		totalReceived++;

		if ((uint8_t)c != expected) {
			Log.info("invalid data got=%02x expected=%02x totalReceived=%u - restarting sequence", c, expected, totalReceived);

			uint8_t next = (uint8_t) (bsdLcgRand() & 0xff);
			if ((uint8_t)c == next) {
				Log.info("dropped a byte, next byte is %02x", next);
			}

			numReceived = 0;
			maxInBuffer = 0;
			totalReceived = 0;
			resetSequence();
			break;
		}
	}

	if (numRead > maxInBuffer) {
		maxInBuffer = numRead;
	}

	if (millis() - lastDisplay >= 5000) {
		lastDisplay = millis();

		Log.info("numReceived=%u maxInBuffer=%u totalReceived=%u", numReceived, maxInBuffer, totalReceived);

		numReceived = 0;
		maxInBuffer = 0;
	}

	// Handle the serial user interface
	{
		int c = Serial.read();
		if (c >= 0) {
			switch(tolower(c)) {
			case 'a':
				Log.info("delay 10 ms.");
				delay(10);
				break;
			case 'b':
				Log.info("delay 100 ms.");
				delay(100);
				break;
			case 'c':
				Log.info("delay 1s");
				delay(1000);
				break;
			case 'd':
				Log.info("delay 2s");
				delay(2000);
				break;
			case 'h':
				Log.info("Cloud disconnect");
				Particle.disconnect();
				break;
			case 'i':
				Log.info("Cloud connect");
				Particle.connect();
				break;
			}
		}
	}
}


void resetSequence() {
	Serial1.write(0);

	serBuf.readClear();

	// Keep reading for 4 seconds to make sure read buffer is cleared
	unsigned long startTime = millis();
	while(millis() - startTime < 4000) {
		serBuf.read();
	}

	// Restart the sequence
	rseed = 1;
}

// We could just use rand/srand here, but by using a known algorithm this makes it
// easier to do testing between different systems (PC -> Photon/Electron, etc.)
#define RAND_MAX ((1U << 31) - 1)

// https://rosettacode.org/wiki/Linear_congruential_generator#C
int bsdLcgRand() {
	return rseed = (rseed * 1103515245 + 12345) & RAND_MAX;
}

