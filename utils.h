/**
  @file utils.h

  @brief Util functions headers

  MIT License

  Copyright (c) 2024 Oleksii Sylichenko

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "globals.h"

#define KELVIN_TO_CELSIUS(k) (k - 273.15)

/**
  Put defined string into program memory,
  in the runtime read the string from the program memory and print it.
*/
#define PGM_PRINT(s) Serial.print(stringFromProgmem(PSTR(s)))

/**
  Put defined string into program memory,
  in the runtime read the string from the program memory and print it.
*/
#define PGM_PRINTLN(s) Serial.println(stringFromProgmem(PSTR(s)))

/**
  Check whether length is greater than 0 and lower that 32.

  Max Data length in the Block Protocol is 32.
*/
bool isAllowedRequestPayloadSize(int len);

int sendCommand(byte command);

/**
  Send word command in Little Endian to the register.

  Bytes of command should be in normal ordering.
  Ordering will be changed to Little Endian by the function.

  0x4321 to REG:
  > write: [ REG, 0x21, 0x43 ]
*/
int sendCommand(byte reg, word command);

/**
  Send byte array data with specified length into specified register.

  Order of the bytes in the data array should be prepared to sending.

  The length should not be greater than 32 and less than 1.
*/
int sendData(byte reg, byte *data, int len);

/**
  Request the device for 36 bytes per multiple requests using the Block Protocol.
  - The first 2 bytes represent the requested address.
  - 32 bytes contain data.
  - 1 byte is allocated for the checksum.
  - 1 byte denotes the total length.
*/
int requestBlock(byte *buf);

byte requestByte();

/**
  Request the device for len bytes per single request.

  Length of the requested data must not be greater than 32,
  because it cannot response more.
*/
int requestBytes(byte *buf, int len);

/**
  Read word from the Device in Little Endian and return as normal word.

  If response: [0xaa, 0xAA] then return: 0xAAaa
*/
word requestWord();

/**
  The checksum is the bitwise inversion of the sum of bytes.

  For example:
  <pre>
    ~(0x35 + 0x00 + 0x23 + 0x01 + 0x67 + 0x45 + 0xAB + 0x89 + 0xEF + 0xCD)
    = ~(0xF5) = ~(0b11110101) = 0b00001010 = 0x0A
  </pre>

  @see https://github.com/tibms/kernel-4.4/blob/release/drivers/power/bq28z610/bq28z610_fg.c#L378
*/
byte checksum(byte *data, byte len);

/**
  Validate the data via checksum: the sum of the data bytes and the checksum should equal a full byte.

  Length of the data array according to the Block Protocol should be equal 36.

  The last two bytes should represent the checksum and the total length.
*/
bool validate(byte *data);

/**
  Compose the word value from the two bytes of the buffer.

  @param buf - data array
  @param lowerByteIndex - Index of the byte that should be placed in the lower position of the result.
  @param littleEndian - Ending indicator:
         - true - Little Endian
         - false - Big Endian
  @returns
          - Little endian: [0x11, 0x22] -> 0x2211
          - Big Endian:    [0x11, 0x22] -> 0x1122
*/
word composeWord(byte *buf, int lowerByteIndex = 0, bool littleEndian = true);

/**
  Returns the Unsigned integer 32-bit value of 4 bytes from the buffer in Little Endian format.

  @param buf - data array
  @param a - index of the least significant byte; bits: [0..7]
  @param b - index of the most significant byte

  @returns
          - [0x11, 0x22, 0x33, 0x44] -> 0x44332211
          - [0x11, 0x22, 0x00, 0x00] -> 0x00002211
*/
u32 composeDoubleWord(byte *buf);

/**
  Compose value from the bytes of the buffer in Little Endian format.

  @param buf - data array
  @param from - index of the least significant byte; bits: [0..7]
  @param till - index of the most significant byte

  @returns
          - [0x11, 0x22, 0x33, 0x44] -> 0x44332211
          - [0x11, 0x22, 0x00, 0x00] -> 0x00002211
*/
u32 composeValue(byte *buf, int from, int till);

/**
  Read string from PROGMEM and return as String in RAM.
*/
String stringFromProgmem(PGM_P stringPtr);

/**
  Print value in binary format with leading zeros.
*/
void printBin(u32 val, int n, bool newLine = false);

/**
  Print long value bits (32) in two lines:
  31 - 16
  15 - 0
*/
void printLongSplitBin(u32 val);
void printLongBin(u32 val, bool newLine = false);
void printWordBin(word val, bool newLine = false);
void printByteBin(byte val, bool newLine = false);

/**
  Print value in the HEX format with leading 0x and zero if necessary.
*/
void printLongHex(u32 val, bool newLine = false);
void printWordHex(word val, bool newLine = false);
void printWordHex(PGM_P caption, word val, bool newLine = true);
void printByteHex(byte val, bool newLine = false);

/**
  Print content of the byte array in one line in hex format with leading zeros if necessary.
  [ AA BB CC DD ... ]
*/
void printBytesHex(byte *buf, int len);

/**
  Print in format: "Caption: value"
*/
void printInteger(PGM_P caption, int value, bool newLine = true);

/**
  Print in format: "Caption: value units"
*/
void printInteger(PGM_P caption, int value, PGM_P units, bool newLine = true);

/**
  Print in format: "value units"
*/
void printInteger(int value, PGM_P units, bool newLine = false);

void printFloat(float value, int format, PGM_P units, bool newLine = false);
void printFloat(PGM_P caption, float value, int format, PGM_P units);
void printFloat(PGM_P caption, float value, int format, PGM_P (*unitsFn)());

/**
  Print the integer value as a float divided by 1000 with 3 decimal places in format:
  "Caption: 65.536"
*/
void printPremil(PGM_P caption, int value, PGM_P units);
void printPremil(PGM_P caption, int value, PGM_P (*unitsFn)());

/**
  Print the Flag number with the caption.
*/
void printFlag(PGM_P caption, u32 flags, int n);

/**
  Print the Flag value with the caption.
  The Flag supplier function is passed as a parameter.
*/
void printFlag(PGM_P caption, u32 flags, Flag (*flagFn)());

/**
  Print the Flag value with the caption.
*/
void printFlag(PGM_P caption, u32 flags, Flag flag);

/**
  Print the Flag value, caption is taken from the Flag definition.
*/
void printFlag(u32 flags, Flag flag);
