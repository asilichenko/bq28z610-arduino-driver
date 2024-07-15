/**
  @file utils.cpp

  @brief Util functions implementation

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

#include "utils.h"

/**
  Check whether length is greater than 0.
*/
bool _isRequestMinAllowed(int len) {
  const bool retval = len > 0;
  if (!retval) PGM_PRINTLN("Request size must be greater than 0.");
  return retval;
}

/**
  Check whether length is greater than 0 and lower that 32.
  This check is useful before single request, because the device
  cannot response more that 32 bytes at once.
*/
bool _isAllowedRequestSize(int len) {
  if (!_isRequestMinAllowed(len)) return false;

  const bool retval = len <= BlockProtocol::REQUEST_MAX_SIZE;
  if (!retval) printInteger(PSTR("The maximum number of bytes that can be obtained per request: "), BlockProtocol::REQUEST_MAX_SIZE);

  return retval;
}

/**
  Check whether length is greater than 0 and lower that 32.

  Max Data length in the Block Protocol is 32.
*/
bool isAllowedRequestPayloadSize(int len) {
  if (!_isRequestMinAllowed(len)) return false;

  const bool retval = len <= BlockProtocol::PAYLOAD_MAX_SIZE;
  if (!retval) printInteger(PSTR("The maximum number of data bytes that can be obtained: "), BlockProtocol::PAYLOAD_MAX_SIZE);

  return retval;
}

int sendCommand(byte command) {
  Wire.beginTransmission(DEVICE_ADDR);
  Wire.write(command);
  return Wire.endTransmission();
}

/**
  Sending word command in Little Endian to the register.

  Bytes of command should be in normal ordering.
  Ordering will be changed to Little Endian by the function.

  0x4321 to REG:
  > write: [ REG, 0x21, 0x43 ]
*/
int sendCommand(byte reg, word command) {
  const byte buf[] = {  // [ 0x..XX, 0xXX.. ]
    command & 0xFF,  // 0x..XX
    (command >> 8) & 0xFF  // 0xXX..
  };
  Wire.beginTransmission(DEVICE_ADDR);
  Wire.write(reg);
  Wire.write(buf, sizeof(buf));
  return Wire.endTransmission();
}

/**
  Send byte array data with specified length into specified register.

  Order of the bytes in the data array should be prepared to sending.

  The length should not be greater than 32 and less than 1.
*/
int sendData(byte reg, byte *data, int len) {
  Wire.beginTransmission(DEVICE_ADDR);
  Wire.write(reg);
  Wire.write(data, len);
  return Wire.endTransmission();
}

/**
  Request the device for 36 bytes per multiple requests using the Block Protocol.
  - The first 2 bytes represent the requested address.
  - 32 bytes contain data.
  - 1 byte is allocated for the checksum.
  - 1 byte denotes the total length.
*/
int requestBlock(byte *buf) {
  int actual = 0;

  byte *bufPtr = buf;
  actual += requestBytes(bufPtr, BlockProtocol::ADDR_SIZE);

  bufPtr += BlockProtocol::ADDR_SIZE;
  actual += requestBytes(bufPtr, BlockProtocol::PAYLOAD_MAX_SIZE);

  bufPtr += BlockProtocol::PAYLOAD_MAX_SIZE;
  actual += requestBytes(bufPtr, BlockProtocol::CHECKSUM_AND_LENGTH_SIZE);

  return actual;
}

byte requestByte() {
  Wire.requestFrom(DEVICE_ADDR, 1);
  return Wire.available() ? Wire.read() : 0;
}

/**
  Request the device for len bytes per single request.
  Length of the requested data must not be greater than 32,
  because it cannot response more.
*/
int requestBytes(byte *buf, int len) {
  if (!_isAllowedRequestSize(len)) return;

  int actual = 0;

  Wire.requestFrom(DEVICE_ADDR, len);
  while (Wire.available() && actual < len) buf[actual++] = Wire.read();

  return actual;
}

/**
  Read word from the Device in Little Endian and return as normal word.

  If response: [0xaa, 0xAA] then return: 0xAAaa
*/
word requestWord() {
  byte buf[] = {0, 0};
  requestBytes(buf, sizeof(buf));
  return composeWord(buf);
}

/**
  The checksum is the bitwise inversion of the sum of bytes.

  For example:
  <pre>
    ~(0x35 + 0x00 + 0x23 + 0x01 + 0x67 + 0x45 + 0xAB + 0x89 + 0xEF + 0xCD)
    = ~(0xF5) = ~(0b11110101) = 0b00001010 = 0x0A
  </pre>

  @see https://github.com/tibms/kernel-4.4/blob/release/drivers/power/bq28z610/bq28z610_fg.c#L378
*/
byte checksum(byte *data, byte len) {
  byte sum = 0;
  for (int i = 0; i < len; i++) sum += data[i];
  return ~sum;
}

/**
  Validate the data via checksum: the sum of the data bytes and the checksum should equal a full byte.
  Length of the data array according to the Block Protocol should be equal 36.
  The last two bytes should represent the checksum and the total length.
*/
bool validate(byte *data) {
  byte sum = data[BlockProtocol::CHECKSUM_INDEX];

  const byte len = data[BlockProtocol::LENGTH_INDEX];
  for (int i = 0; i < len - 2; i++) sum += data[i];  // exclude length byte itself

  const bool retval = (sum & 0xFF);
  if (!retval) PGM_PRINTLN("The device responded with invalid data.");

  if (DEBUG) {
    printInteger(PSTR("$Checksum"), data[BlockProtocol::CHECKSUM_INDEX]);
    printInteger(PSTR("$Length"), data[BlockProtocol::LENGTH_INDEX]);
    printInteger(PSTR("$Result"), sum);
  }

  return retval;
}

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
word composeWord(byte *buf, int lsbIndex, bool littleEndian) {
  const int msbIndex = lsbIndex + (littleEndian ? 1 : -1);
  if (msbIndex < 0) {
    PGM_PRINTLN("~ Error: Incorrect pair of index and endian.");
    return 0;
  }
  return (buf[msbIndex] << 8) | buf[lsbIndex];
}

/**
  Returns the Unsigned integer 32-bit value of 4 bytes from the buffer in Little Endian format.

  @param buf - data array
  @param a - index of the least significant byte; bits: [0..7]
  @param b - index of the most significant byte

  @returns
          - [0x11, 0x22, 0x33, 0x44] -> 0x44332211
          - [0x11, 0x22, 0x00, 0x00] -> 0x00002211
*/
u32 composeDoubleWord(byte *buf) {
  return composeValue(buf, 0, 3);
}

/**
  Compose value from the bytes of the buffer in Little Endian format.

  @param buf - data array
  @param from - index of the least significant byte; bits: [0..7]
  @param till - index of the most significant byte

  @returns
          - [0x11, 0x22, 0x33, 0x44] -> 0x44332211
          - [0x11, 0x22, 0x00, 0x00] -> 0x00002211
*/
u32 composeValue(byte *buf, int from, int till) {
  if (till <= from) {
    PGM_PRINTLN("~ Error: Invalid range to compose value. Till param should be greater than From param.");
    return;
  }

  u32 retval = 0;
  for (int i = till; i >= from; i--) {
    retval = retval << 8;
    retval |= buf[i];
  }
  return retval;
}

/**
  Read string from PROGMEM and return as String in RAM.
*/
String stringFromProgmem(PGM_P stringPtr) {
  const char buf[strlen_P(stringPtr) + 1];
  strcpy_P(buf, stringPtr);
  return String(buf);
}

/**
  Print value in the HEX format with leading 0x and zero if necessary.
*/
void printLongHex(u32 val, bool newLine) {
  PGM_PRINT("0x");
  Serial.print(val, HEX);
  if (newLine) Serial.println();
}

void printWordHex(word val, bool newLine) {
  PGM_PRINT("0x");
  if (val <= 0xFFF) PGM_PRINT("0");
  if (val <= 0xFF) PGM_PRINT("0");
  if (val <= 0xF) PGM_PRINT("0");
  Serial.print(val, HEX);
  if (newLine) Serial.println();
}

void printByteHex(byte val, bool newLine) {
  PGM_PRINT("0x");
  if (val <= 0xF) PGM_PRINT("0");
  Serial.print(val, HEX);
  if (newLine) Serial.println();
}

/**
  Print content of the byte array in one line in hex format with leading zeros if necessary.
  [ AA BB CC DD ... ]
*/
void printBytesHex(byte *buf, int len) {
  PGM_PRINT("[ ");
  for (int i = 0; i < len; i++) {
    byte val = buf[i];
    if (val <= 0xF) PGM_PRINT("0");
    Serial.print(val, HEX);
    PGM_PRINT(" ");
  }
  PGM_PRINTLN("]");
}

/**
  Print value in binary format with leading zeros.
*/
void printBin(u32 val, int n, bool newLine) {
  for (int i = n - 1; i >= 0; i--) Serial.print(bitRead(val, i));
  if (newLine) Serial.println();
}

/**
  Print long value bits (32) in two lines:
  31 - 16
  15 - 0
*/
void printLongSplitBin(u32 val) {
  printWordBin((val >> 16) & 0xFFFF, true);  // 31 - 16
  printWordBin(val & 0xFFFF, true);  // 15 - 0
}

void printLongBin(u32 val, bool newLine) {
  printBin(val, 32, newLine);
}

void printWordBin(word val, bool newLine) {
  printBin(val, 16, newLine);
}

void printByteBin(byte val, bool newLine) {
  printBin(val, 8, newLine);
}

void __printPgm(PGM_P pgmPtr) {
  const String s = stringFromProgmem(pgmPtr);
  Serial.print(s);
}

void __printlnPgm(PGM_P pgmPtr) {
  const String s = stringFromProgmem(pgmPtr);
  Serial.println(s);
}

void __printCaption(PGM_P caption) {
  __printPgm(caption);
  PGM_PRINT(": ");
}

void __printUnits(PGM_P units, bool newLine) {
  PGM_PRINT(" ");
  __printPgm(units);
  if (newLine) Serial.println();
}

void printInteger(int value, PGM_P units, bool newLine) {
  Serial.print(value);
  __printUnits(units, newLine);
}

void printInteger(PGM_P caption, int value, PGM_P units, bool newLine) {
  __printCaption(caption);
  printInteger(value, units, newLine);
}

void printInteger(PGM_P caption, int value, bool newLine) {
  __printCaption(caption);
  Serial.print(value);
  if (newLine) Serial.println();
}

void printFloat(float value, int format, PGM_P units, bool newLine) {
  Serial.print(value, format);
  __printUnits(units, newLine);
}

void printFloat(PGM_P caption, float value, int format, PGM_P units) {
  __printCaption(caption);
  printFloat(value, format, units, true);
}

void printFloat(PGM_P caption, float value, int format, PGM_P (*unitsFn)()) {
  printFloat(caption, value, format, (*unitsFn)());
}

/**
  Print the integer value as a float divided by 1000 with 3 decimal places in format:
  "Caption: 65.536"
*/
void printPremil(PGM_P caption, int value, PGM_P units) {
  const float fValue = PERMIL * value;
  printFloat(caption, fValue, PERMIL_DECIMAL, units);
}

void printPremil(PGM_P caption, int value, PGM_P (*unitsFn)()) {
  printPremil(caption, value, (*unitsFn)());
}

void printFlag(PGM_P caption, u32 flags, int n) {
  __printCaption(caption);
  Serial.println(bitRead(flags, n));
}

/**
  Print the Flag value with the caption.
  The Flag supplier function is passed as a parameter.
*/
void printFlag(PGM_P caption, u32 flags, Flag (*flagFn)()) {
  const Flag flag = (*flagFn)();
  printFlag(caption, flags, flag.n);
}

void printFlag(PGM_P caption, u32 flags, Flag flag) {
  printFlag(caption, flags, flag.n);
}

void printFlag(u32 flags, Flag flag) {
  printFlag(flag.caption, flags, flag.n);
}

void printWordHex(PGM_P caption, word val, bool newLine) {
  __printCaption(caption);
  printWordHex(val, newLine);
}
