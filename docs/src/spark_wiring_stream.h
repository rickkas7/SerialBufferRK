/**
 ******************************************************************************
 * @file    spark_wiring_stream.h
 * @author  Mohit Bhoite
 * @version V1.0.0
 * @date    13-March-2013
 * @brief   Header for spark_wiring_stream.c module
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.
  Copyright (c) 2010 David A. Mellis.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
  ******************************************************************************
 */

#ifndef __SPARK_WIRING_STREAM_H
#define __SPARK_WIRING_STREAM_H

#include "spark_wiring_string.h"
#include "spark_wiring_print.h"
#include "system_tick_hal.h"

// compatability macros for testing
/*
#define   getInt()            parseInt()
#define   getInt(skipChar)    parseInt(skipchar)
#define   getFloat()          parseFloat()
#define   getFloat(skipChar)  parseFloat(skipChar)
#define   getString( pre_string, post_string, buffer, length)
readBytesBetween( pre_string, terminator, buffer, length)
*/

/**
 * @brief Arduino/Wiring Stream Class
 *
 * Methods for reading data from a stream, such a serial port, TCP network stream, or a file.
 */
class Stream : public Print
{
  protected:
#ifndef DOXYGEN_DO_NOT_DOCUMENT
    system_tick_t _timeout;      // number of milliseconds to wait for the next char before aborting timed read
    system_tick_t _startMillis;  // used for timeout measurement
    int timedRead();    // private method to read stream with timeout
    int timedPeek();    // private method to peek stream with timeout
    int peekNextDigit(); // returns the next numeric digit in the stream or -1 if timeout
#endif

  public:
    /**
     * @brief Returns the number of a bytes available to read right now.
     *
     * For files, the number of bytes left to read in the file from the current file position.
     */
    virtual int available() = 0;

    /**
     * @brief Read a byte from the stream
     *
     * @return A byte value 0 - 255, or -1 if there are no bytes available.
     */
    virtual int read() = 0;

    /**
     * @brief Read a byte from the stream but do not remove it so read will return the same byte next time.
     *
     * @return A byte value 0 - 255, or -1 if there are no bytes available.
     *
     * For files, the current file position is left unchanged.
     */
    virtual int peek() = 0;

    /**
     * @brief For output streams, writes any unwritten buffered data
     */
    virtual void flush() = 0;

#ifndef DOXYGEN_DO_NOT_DOCUMENT
    Stream() {_timeout=1000;}
#endif

// parsing methods

    /**
     * @brief Sets the read timeout (default: 1000 milliseconds)
     *
     * This makes more sense for network and serial streams.
     */
  void setTimeout(system_tick_t timeout);  // sets maximum milliseconds to wait for stream data, default is 1 second

  /**
   * @brief Reads data from the stream until the target string is found
   *
   * @param target The string to search for (null-terminated c-string)
   *
   * @return true if target string is found, false if timed out (see setTimeout)
   */
  bool find(char *target);

  /**
   * @brief Reads data from the stream until the target string is found
   *
   * @param target The string to search for. The string is specified by length so it does not need to be null-terminated.
   *
   * @param length The number of bytes in target to search for.
   *
   * @return true if target string is found, false if timed out (see setTimeout)
   */
  bool find(char *target, size_t length);
  /**
   * @brief Reads data from the stream until the target string is found or the terminator string is found
   *
   * @param target The string to search for (null-terminated c-string)
   *
   * @param terminator The terminator string to search for (null-terminated c-string)
   *
   * @return true if target string is found, false if timed out (see setTimeout)
   */
  bool findUntil(char *target, char *terminator);

  /**
   * @brief Reads data from the stream until the target string is found or the terminator string is found
   *
   * @param target The string to search for. The string is specified by length so it does not need to be null-terminated.
   *
   * @param length The number of bytes in target to search for.
   *
   * @param terminator The terminator string to search for. The string is specified by length so it does not need to be null-terminated.
   *
   * @param termLen The number of bytes in terminator to search for.
   *
   * @return true if target string is found, false if timed out (see setTimeout)
   */
  bool findUntil(char *target, size_t targetLen, char *terminate, size_t termLen);   // as above but search ends if the terminate string is found


  /**
   * @brief returns the first valid (long) integer value from the current position
   *
   * Initial characters that are not digits (or the minus sign) are skipped
   * integer is terminated by the first character that is not a digit.
   */
  long parseInt();


  /**
   * @brief returns the first valid float value from the current position
   *
   * Initial characters that are not digits (or the minus sign or .) are skipped
   * float is terminated by the first character that is not a float character.
   * Decimal part is separate by '.'.
   */
  float parseFloat();

  /**
   * @brief Read chars from stream into buffer
   *
   * @param buffer The buffer to read into
   * @param length The number of bytes to read
   * @return the number of characters placed in the buffer (0 means no valid data found)
   */
  size_t readBytes( char *buffer, size_t length);

  /**
   * @brief Read chars from stream into buffer until the character terminator is found
   *
   * @param terminator The character to stop reading
   * @param buffer The buffer to read into
   * @param length The number of bytes to read
   * @return the number of characters placed in the buffer (0 means no valid data found)
   *
   * The terminator could be some thing like \\n (newline), \\t (tab), etc. depending on the data you are parsing.
   */
  size_t readBytesUntil( char terminator, char *buffer, size_t length);

  // Arduino String functions to be added here

  /**
   * @brief Reads the remainder of the file into a string.
   */
  String readString();

  /**
   * @brief Reads the remainder of the file into a string or until terminator is found.
   *
   * @param terminator The character to stop reading at.
   */
  String readStringUntil(char terminator);

  protected:
#ifndef DOXYGEN_DO_NOT_DOCUMENT
  long parseInt(char skipChar); // as above but the given skipChar is ignored
  // as above but the given skipChar is ignored
  // this allows format characters (typically commas) in values to be ignored

  float parseFloat(char skipChar);  // as above but the given skipChar is ignored
#endif
};

#endif
