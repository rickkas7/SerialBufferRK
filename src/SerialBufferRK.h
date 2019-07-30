#ifndef __SERIALBUFFERRK_H
#define __SERIALBUFFERRK_H

/**
 * Home: https://github.com/rickkas7/SerialBufferRK
 * License: MIT
 */

#include "Particle.h"
#include "RingBuffer.h"

/**
 * @brief Buffered Serial Class
 *
 * Inherits from Stream so you can use the standard Wiring/Arduino stream features as well,
 * like readLine.
 */
class SerialBufferBase : public Stream {
public:
	/**
	 * @brief Constructs a SerialBufferBase object. You normally will use SerialBuffer instead.
	 *
	 * @param buf Pointer to the buffer to hold the data
	 *
	 * @param bufSize The size of the buffer in bytes
	 *
	 * @param port The real serial port to buffer
	 *
	 */
	SerialBufferBase(uint8_t *buf, size_t bufSize, USARTSerial &port);

	/**
	 * @brief You must call setup() from setup()!
	 *
	 * This call was added in version 0.0.2 of the library do solve an incompatibility
	 * with thread creation from a global object, mainly on Gen 3 devices with newer Device OS
	 * versions.
	 */
	void setup();

	/**
	 * @brief Returns the number of bytes available to read from the serial port
	 *
	 * This is a standard Arduino/Wiring method for Stream objects.
	 */
    virtual int available();

	/**
	 * @brief Returns the number of bytes available to write into the TX FIFO
	 *
	 * You can safely use the availableForWrite method of the actual serial port object instead of this
	 * one if you prefer.
	 */
    virtual int availableForWrite();

	/**
	 * @brief Read a single byte from the serial port
	 *
	 * @return a byte value 0 - 255 or -1 if no data is available.
	 *
	 * This is a standard Arduino/Wiring method for Stream objects.
	 */
    virtual int read();

	/**
	 * @brief Read a single byte from the serial port, but do not remove it so it can be read again.
	 *
	 * @return a byte value 0 - 255 or -1 if no data is available.
	 *
	 * This is a standard Arduino/Wiring method for Stream objects.
	 */
    virtual int peek();

    /**
     * @brief Clear the read buffer
     */
    void readClear();

	/**
	 * @brief Block until all serial data is sent.
	 *
	 * This is a standard Arduino/Wiring method for Stream objects.
	 *
	 * You can safely use the flush method of the actual serial port object instead of this
	 * one if you prefer.
	 */
    virtual void flush();

	/**
	 * @brief Write a single byte to the serial port.
	 *
	 * @param c The byte to write. Can write binary or text data.
	 *
	 * This is a standard Arduino/Wiring method for Stream objects.
	 *
	 * You can safely use the write method of the actual serial port object instead of this
	 * one if you prefer.
	 */
    virtual size_t write(uint8_t c);


private:
    /**
     * @brief Thread function called from FreeRTOS. Never returns!
     */
    void threadFunction();

    /**
     * @brief Static thread function, called from FreeRTOS
     *
     * Note: param must be a pointer to this. threadFunction is called from this function.
     * Never returns!
     */
    static void threadFunctionStatic(void *param);

	uint8_t *buf;
	size_t bufSize;
	USARTSerial &port;
	RingBuffer<uint8_t> ring;
	os_thread_t thread;
};


/**
 * @brief Buffered serial class
 *
 * @param SIZE the size of the buffer in bytes
 *
 * @param port The port to attach to. Must be a USARTSerial (Serial1, Serial2, Serial4, ...)
 *
 * The buffered serial class creates a thread to read from the serial port, eliminating problems with
 * dropping data
 */
template <size_t SIZE>
class SerialBuffer : public SerialBufferBase {
public:
	/**
	 * @brief Constructs a SerialBuffer object
	 *
	 * @param port The real serial port to get the data to buffer from.
	 */
	SerialBuffer(USARTSerial &port) : SerialBufferBase(staticBuffer, SIZE, port) {
	}

private:
	uint8_t staticBuffer[SIZE];
};



#endif /* __SERIALBUFFERRK_H */
