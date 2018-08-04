# SerialBufferRK

*Serial buffer for the Particle Photon, P1, and Electron to make dealing with receiving data, especially at higher data rates, more reliable by implementing a larger buffer.*

[Full browsable API documentation](http://rickkas7.github.io/SerialBufferRK/) is available.

SerialBufferRK creates a thread to continuously read from the hardware serial port and copy the data into a thread-safe ring buffer. You can choose the size of the ring buffer.

You could then read the data at your leisure out of loop. The USART serial buffer is only 64 bytes, but by using a much larger buffer (say, 4096 bytes or even larger) you can greatly reduce the likelihood of lost data.

## Using it

If you are using Particle Build (Web IDE), search for the library `SerialBufferRK` and add it.

From the Particle CLI you can add the library to your project (CLI or Particle Dev Atom IDE) by using:

```
particle library add SerialBufferRK
```

Add the header file if necessary:

```
#include "SerialBufferRK.h"
```

In many cases you will use it with system thread enabled mode. It's not required, however. While the serial buffer uses threads, it does not require the system thread.

```
SYSTEM_THREAD(ENABLED);
```

You need to allocate a SerialBuffer object, typically as a global variable.

```
SerialBuffer<4096> serBuf(Serial1);
```

This allocates a 4096 byte serial buffer connected to Serial1.

Whenever you would read from Serial1, you should instead read from serBuf. This includes using things like available() and read() as well as the Wiring/Aruduino functions like find(), readStringUntil(), etc.

```
	int c = serBuf.read();
```

You should read all of the bytes available, calling until read() return -1 or use available() to determine if there are available bytes to read.

You do not need to use serBuf for writing, though you can if you want.

Make sure you never mix reads from serBuf and the actual serial port (Serial1, etc.) as the data will be corrupted.

You still need to initialize the serial port, you should do that from setup():

```
	Serial1.begin(230400);
```

## Full Example

```
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

```

## Test Code

In the examples directory, there are two examples for testing:

- 2-test-receiver-SerialBufferRK
- 3-test-sender-SerialBufferRK

You can test the library by connecting two devices RX -> TX and TX -> RX and running one sender and one receiver.

The sender and receiver use the same linear congruential generator pseudo-random number generator with the same seed, so the data can be verified by lots of different bit change sequences are tested.

Monitor the serial output on the receiver.

```
0000948089 [app] INFO: numReceived=115385 maxInBuffer=45 totalReceived=21669298
0000953089 [app] INFO: numReceived=115385 maxInBuffer=46 totalReceived=21784683
0000958089 [app] INFO: numReceived=115385 maxInBuffer=45 totalReceived=21900068
0000963089 [app] INFO: numReceived=115385 maxInBuffer=43 totalReceived=22015453
```


