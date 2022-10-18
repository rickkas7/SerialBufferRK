/**
 * Home: https://github.com/rickkas7/SerialBufferRK
 * License: MIT
 */

#include "SerialBufferRK.h"

SerialBufferBase::SerialBufferBase(uint8_t *buf, size_t bufSize, USARTSerial &port) :
	buf(buf), bufSize(bufSize), port(port), ring(buf, bufSize) {

}

void SerialBufferBase::setup() {
	os_thread_create(&thread, "serialBuffer", OS_THREAD_PRIORITY_DEFAULT, threadFunctionStatic, (void *)this, 512);
}

int SerialBufferBase::available() {
	return ring.availableForRead();
}

int SerialBufferBase::availableForWrite() {
	return port.availableForWrite();
}

int SerialBufferBase::read() {
	uint8_t temp;

	if (ring.read(&temp)) {
		return (int) temp;
	}
	else {
		// No data to read
		return -1;
	}
}

int SerialBufferBase::peek() {
	uint8_t *p = ring.preRead();
	if (p != NULL) {
		return (int) *p;
	}
	else {
		return -1;
	}
}

void SerialBufferBase::readClear() {
	ring.readClear();
}

void SerialBufferBase::flush() {
	port.flush();
}

size_t SerialBufferBase::write(uint8_t c) {
	return port.write(c);
}


void SerialBufferBase::threadFunction() {
	while(true) {
		// Consume all of the outstanding data on the hardware port before yielding as long as there
		// is room in the ring buffer
		while(true) {
			uint8_t *p = ring.preWrite();
			if (!p) {
				// Ring buffer is full
				break;
			}

			int c = port.read();
			if (c < 0) {
				break;
			}

			*p = (uint8_t) c;
			ring.postWrite();
		}

		os_thread_yield();
	}
}

// [static]
void SerialBufferBase::threadFunctionStatic(void *param) {
	SerialBufferBase *This = (SerialBufferBase *)param;

	This->threadFunction();
}


