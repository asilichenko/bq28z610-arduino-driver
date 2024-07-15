/**
  @file data_flash_access.cpp

  @brief 12.2.45 Data Flash Access() 0x4000–0x5FFF implementation

  Accessing data flash (DF) is only supported by the AltManufacturerAccess() by addressing the physical address.

  @warning [!] Not Available in SEALED Mode.
               UNSEALED Mode is enough for both: reading and writing.
               If you try to read the DF in SEALED mode, you'll just get an array of zeros.

  <pre>
  BQ28Z610
  Technical Reference Manual
  (c) Texas Instruments
  Literature Number: SLUUA65E
  </pre>

  @see https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf
  @see 13 Data Flash Values
  @see 14 Data Flash Summary


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

#include "data_flash_access.h"

/**
  @brief Checks if requested address fits into the Data Flash addresses region.
*/
bool _isAddrValid(word addr) {
  const bool retval = DF_ADDR::MIN <= addr && addr <= DF_ADDR::MAX;
  if (!retval) {
    PGM_PRINT("Address must be in the range: [");
    Serial.print(DF_ADDR::MIN);
    PGM_PRINT("-");
    Serial.print(DF_ADDR::MAX);
    PGM_PRINTLN("]");
  }
  return retval;
}

/**
  @brief Operations with Data Flash are not allowed in the Sealed mode.
  @see securityMode()
*/
bool _isDeviceSealed() {
  const bool silence = SILENCE;
  SILENCE = true;
  const bool retval = SecurityMode::SEALED == securityMode();
  SILENCE = silence;

  if (retval) PGM_PRINTLN("[!] Operations with Data Flash are not allowed in SEALED Mode.");
  return retval;
}

/**
  @brief Read array of bytes from the Data Flash by address.

  Address should be fit into Data Flash addresses region, otherwise error message will be printed.

  Requested length should be in the range [1; 32]

  @see AltManufacturerAccess()
  @see DF_ADDR::MIN
  @see DF_ADDR::MAX
  @see BlockProtocol
*/
void dfReadBytes(word addr, byte *retval, int len) {
  if (!_isAddrValid(addr) || !isAllowedRequestPayloadSize(len) || _isDeviceSealed()) return;

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], _len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(addr, buf, &_len)) return;

  for (int i = 0; i < len; i++) retval[i] = buf[i];
}

/**
  @brief Write array of data to the Data Flash.

  The byte order should be as it should be written in the data flash.

  The Data length should be in the range [1; 32]

  Some addresses within the standard region are prohibited from writing,
  I am not sure about the exact ranges of such addresses.

  For example, around the address 0x5A5A, there are data 0xFF and they cannot be overwritten.

  Special thanks for providing the accurate definition of the DF writing algorithm
  to the user "silomilo" from the Texas Intruments forum:
  - https://e2e.ti.com/support/power-management-group/power-management/f/power-management-forum/738029/bq28z610-data-flash-access-issues/2726302#2726302

  > Write to 0x3E ( AltManufacturingAccess) the MAC address (little endian)
  > followed by the data to write ( Note: Minimum data is 1 and maximum is 32 )
  >
  > Write to 0x60 ( MacDataChecksum ) the checksum calculated as per below
  >
  > Write to 0x61 ( MacDataLength ) the total number of bytes written
  > including the MAC address, data bytes, checksum, and MACDataLength itself
  >
  > Read back from MAC address to verify (Write address to read to 0x3E and read back desired number of bytes)

  @param addr - Address of the Data Flash to write data.
  @param data - Array of data bytes.
  @param len - Length of the data.

  @see StdCommands::ALT_MANUFACTURER_ACCESS
  @see StdCommands::MAC_DATA_CHECKSUM
*/
void dfWriteBytes(word addr, byte *data, int len) {
  if (!_isAddrValid(addr) || !isAllowedRequestPayloadSize(len) || _isDeviceSealed()) return;

  byte buf[BlockProtocol::ADDR_SIZE + len];
  buf[0] = addr & 0xFF;
  buf[1] = (addr >> 8) & 0xFF;
  for (int i = 0; i < len; i++) buf[BlockProtocol::ADDR_SIZE + i] = data[i];
  sendData(StdCommands::ALT_MANUFACTURER_ACCESS, buf, sizeof(buf));

  const byte _checksum = checksum(buf, sizeof(buf));
  const byte _length = len + BlockProtocol::SERVICE_SIZE;
  const byte checksumAndLength[] = {_checksum, _length};
  sendData(StdCommands::MAC_DATA_CHECKSUM, checksumAndLength, sizeof(checksumAndLength));

  delay(200);
}

/**
  @see dfReadBytes()
*/
byte dfReadByte(word addr) {
  byte value = 0;
  dfReadBytes(addr, &value, 1);
  return value;
}

/**
  @see dfReadByte()
*/
byte dfReadU1(word addr) {
  return dfReadByte(addr);
}

/**
  @see dfWriteBytes()
*/
void dfWriteByte(word addr, byte value) {
  dfWriteBytes(addr, &value, 1);
}

/**
  @brief Read a word value (two bytes) from the Data Flash by address.

  Result is in the Normal Order.

  @param addr - two-byte address
  @returns two byte in normal order: 0x4321

  @see dfReadBytes()
  @see composeWord()
*/
word dfReadWord(word addr) {
  byte data[] = {0, 0};
  dfReadBytes(addr, data, sizeof(data));
  return composeWord(data);
}

/**
  @brief Write a word value (two bytes) to the Data Flash by address.

  Order of bytes should be Normal.

  @see dfWriteBytes()
*/
void dfWriteWord(word addr, word value) {
  const byte data[] = {
    value & 0xFF,
    (value >> 8) & 0xFF,
  };
  dfWriteBytes(addr, data, sizeof(data));
}

/**
  @brief Read Two-byte Integer value from the Data Flash by address.
  @see dfReadWord()
*/
int dfReadI2(word addr) {
  return dfReadWord(addr);
}

/**
  @brief Write int value to the Data Flash by address.
  @see dfWriteWord()
*/
void dfWriteI2(word addr, int value) {
  dfWriteWord(addr, value);
}

/**
  @brief Read Two-byte unsigned integer value from the Data Flash by address.
  @see dfReadWord()
*/
word dfReadU2(word addr) {
  return dfReadWord(addr);
}

/**
  @brief Write unsigned int value to the Data Flash by address.
  @see dfWriteWord()
*/
void dfWriteU2(word addr, word value) {
  dfWriteWord(addr, value);
}

/**
  @brief Read String value from the Data Flash by address.
  @see AltManufacturerAccess()
*/
String dfReadString(word addr) {
  if (!_isAddrValid(addr)) return;

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(addr, buf, &len)) return "";

  const int str_len = buf[0];
  char str[str_len + 1];  // len + '0'
  for (int i = 0; i <= str_len; i++) str[i] = buf[1 + i];

  return String(str);
}

/**
  @brief Read the Device Name from the Data Flash.

  I2C Configuration; Data; 0x4080; Device Name; S21

  @see DF_ADDR::DEVICE_NAME
*/
String dfDeviceName() {
  const String retval = dfReadString(DF_ADDR::DEVICE_NAME);
  if (!SILENCE) {
    PGM_PRINT("=== Device Name: ");
    Serial.println(retval);
  }
  return retval;
}

/**
  @brief Read the Design Capacity in mAh from the Data Flash.

  Gas Gauging; Design; 0x462A; Design Capacity mAh; I2

  Is used for LOAD mode = Constant Current.

  @see DF_ADDR::DESIGN_CAPACITY_MAH
*/
int dfReadDesignCapacityMah() {
  const int retval = dfReadI2(DF_ADDR::DESIGN_CAPACITY_MAH);
  if (!SILENCE) printInteger(PSTR("=== Design Capacity"), retval, Units::MAH());
  return retval;
}

/**
  @brief Write Design Capacity in mAh to the Data Flash.

  Gas Gauging; Design; 0x462A; Design Capacity mAh; I2
  - JBL: 5100

  @see DF_ADDR::DESIGN_CAPACITY_MAH
*/
void dfWriteDesignCapacityMah(int designCapacityMah) {
  dfWriteWord(DF_ADDR::DESIGN_CAPACITY_MAH, designCapacityMah);
}

/**
  @brief Read the Design Energy in cWh from the Data Flash.

  Gas Gauging; Design; 0x462C; Design Capacity cWh; I2

  Is used for LOAD mode = Constant Power.

  @see DF_ADDR::DESIGN_CAPACITY_CWH
*/
int dfReadDesignCapacityCwh() {
  const int retval = dfReadI2(DF_ADDR::DESIGN_CAPACITY_CWH);
  if (!SILENCE) printInteger(PSTR("=== Design Capacity"), retval, Units::CWH());
  return retval;
}

/**
  @brief Write Design Energy in cWh to the Data Flash.

  Gas Gauging; Design; 0x462C; Design Capacity cWh; I2
  - JBL: 3672

  Design Energy = Nominal Voltage * Capacity
  7.2 V * 5100 mAh = 36'720 mWh = 3672 cWh

  @see DF_ADDR::DESIGN_CAPACITY_CWH
*/
void dfWriteDesignCapacityCwh(int designCapacityCwh) {
  dfWriteWord(DF_ADDR::DESIGN_CAPACITY_CWH, designCapacityCwh);
}

/**
  @brief Settings; Configuration; 0x4600; FET Options; H1

  <pre>
  Bit 0: Reserved
  Bit 1: Reserved
  Bit 2: OTFET—FET action in OVERTEMPERATURE mode
    0 = No FET action for overtemperature condition
    1 = CHG and DSG FETs will be turned off for overtemperature conditions.
  Bit 3: CHGSU—FET action in CHARGE SUSPEND mode
    0 = FET active
    1 = Charging or precharging disabled, FET off
  Bit 4: CHGIN—FET action in CHARGE INHIBIT mode
    0 = FET active
    1 = Charging or precharging disabled, FET off
  Bit 5: CHGFET—FET action on valid charge termination
    0 = FET active
    1 = Charging or precharging disabled, FET off
  Bit 6: SLEEPCHG—CHG FET enabled during sleep
    0 = CHG FET off during sleep
    1 = CHG FET remains on during sleep.
  Bit 7: Reserved
  </pre>

  @see DF_ADDR::FET_OPTIONS
  @see FetOptionsFlags
*/
byte dfReadFetOptions() {
  const word addr = DF_ADDR::FET_OPTIONS;
  const byte retval = dfReadByte(addr);
  if (!SILENCE) {
    printWordHex(PSTR("\n=== Data Flash [FET Options]"), addr);
    printByteBin(retval, true);
    printFlag(retval, FetOptionsFlags::OTFET());  // Bit 2: OTFET—FET action in OVERTEMPERATURE mode
    printFlag(retval, FetOptionsFlags::CHGSU());  // Bit 3: CHGSU—FET action in CHARGE SUSPEND mode
    printFlag(retval, FetOptionsFlags::CHGIN());  // Bit 4: CHGIN—FET action in CHARGE INHIBIT mode
    printFlag(retval, FetOptionsFlags::CHGFET());  // Bit 5: CHGFET—FET action on valid charge termination
    printFlag(retval, FetOptionsFlags::SLEEPCHG());  // Bit 6: SLEEPCHG—CHG FET enabled during sleep
  }
  return retval;
}

/**
  @brief Settings; Configuration; 0x469B; DA Configuration; H1

  <pre>
  Bit 0: CC0—Cell Count
    0 = 1 cell
    1 = 2 cell
  Bit 1: Reserved
  Bit 2: Reserved
  Bit 3: IN_SYSTEM_SLEEP—In-system SLEEP mode
    0 = Disables (default)
    1 = Enables
  Bit 4: SLEEP—SLEEP Mode
    0 = Disables SLEEP mode
    1 = Enables SLEEP mode (default)
  Bit 5: Reserved
  Bit 6: CTEMP—Cell Temperature protection source
    0 = MAX (default)
    1 = Average
  Bit 7: Reserved
  </pre>

  @see DF_ADDR::DA_CONFIGURATION
  @see DaConfigurationFlags
*/
byte dfReadDaConfiguration() {
  if (!SILENCE) {
    PGM_PRINT("\n=== Data Flash [DA Configuration]: ");
    printWordHex(DF_ADDR::DA_CONFIGURATION, true);
  }
  const byte retval = dfReadByte(DF_ADDR::DA_CONFIGURATION);
  if (!SILENCE) {
    printByteBin(retval, true);
    printFlag(retval, DaConfigurationFlags::CC0());  // Bit 0: CC0—Cell Count
    printFlag(retval, DaConfigurationFlags::IN_SYSTEM_SLEEP());  // Bit 3: IN_SYSTEM_SLEEP—In-system SLEEP mode
    printFlag(retval, DaConfigurationFlags::SLEEP());  // Bit 4: SLEEP—SLEEP Mode
    printFlag(retval, DaConfigurationFlags::CTEMP());  // Bit 6: CTEMP—Cell Temperature protection source
  }
  return retval;
}

/**
  @brief Gas Gauging; Update Status; 0x420E; H1

  <pre>
  Bit 1:0: Update1, Update0 - Update Status:
    0,0 = Impedance Track gauging and lifetime updating is disabled.
    0,1 = QMax updated
    1,0 = QMax and Ra table have been updated
  Bit 2: Impedance Track gauging and lifetime updating:
    0 = Disabled
    1 = Enabled
  Bit 3: QMax update:
    0 = Not updated
    1 = Updated
  Bit 7:4: Reserved
  </pre>

  @warning [!] If any Reserved bit is set then check whether security mode is Unsealed, otherwise Unseal the Device.

  - 0x00 = 0000 - Impedance Track gauging and lifetime updating is disabled.
  - 0x01 = 0001
  - 0x02 = 0010
  - 0x04 = 0100 - Neither QMax nor Ra table are updated. Learning is in progress.
  - 0x05 = 0101 - QMax is updated.
  - 0x06 = 0110 - QMax and Ra table are updated.
  - 0x0E = 1110 - QMax is updated in field (in real conditions).

  @see DF_ADDR::GAS_GAUGING_UPDATE_STATUS
  @see GasGaugingUpdateStatusFlags
*/
byte dfReadGasGaugingUpdateStatus() {
  if (!SILENCE) {
    PGM_PRINT("\n=== Data Flash [Gas Gauging Update Status]: ");
    printWordHex(DF_ADDR::GAS_GAUGING_UPDATE_STATUS, true);
  }
  const byte retval = dfReadByte(DF_ADDR::GAS_GAUGING_UPDATE_STATUS);
  if (!SILENCE) {
    printByteBin(retval);
    PGM_PRINT(" = ");
    printByteHex(retval, true);

    PGM_PRINT("Bit 1:0: Update1, Update0 - Update Status: ");
    Serial.print(bitRead(retval, GasGaugingUpdateStatusFlags::Update1().n));
    Serial.println(bitRead(retval, GasGaugingUpdateStatusFlags::Update0().n));

    printFlag(retval, GasGaugingUpdateStatusFlags::Enable());  // Bit 2: Enable—Impedance Track gauging and lifetime updating is enabled
    printFlag(retval, GasGaugingUpdateStatusFlags::QMax_update());  // Bit 3: QMax_update
  }
  return retval;
}

/**
  @brief Gas Gauging; Update Status; 0x420E; H1

  <pre>
  Bit 1:0: Update1, Update0 - Update Status:
    0,0 = Impedance Track gauging and lifetime updating is disabled.
    0,1 = QMax updated
    1,0 = QMax and Ra table have been updated
  Bit 2: Impedance Track gauging and lifetime updating:
    0 = Disabled
    1 = Enabled
  Bit 3: QMax update:
    0 = Not updated
    1 = Updated
  Bit 7:4: Reserved
  </pre>

  @warning [!] If any Reserved bit is set then check whether security mode is Unsealed, otherwise Unseal the Device.

  - 0x00 = 0000 - Impedance Track gauging and lifetime updating is disabled.
  - 0x01 = 0001
  - 0x02 = 0010
  - 0x04 = 0100 - Neither QMax nor Ra table are updated. Learning is in progress.
  - 0x05 = 0101 - QMax is updated.
  - 0x06 = 0110 - QMax and Ra table are updated.
  - 0x0E = 1110 - QMax is updated in field (in real conditions).

  @see DF_ADDR::GAS_GAUGING_UPDATE_STATUS
*/
void dfWriteGasGaugingUpdateStatus(byte updateStatus) {
  dfWriteByte(DF_ADDR::GAS_GAUGING_UPDATE_STATUS, updateStatus);
}

/**
  @brief Reset "R_a flags" in the Data Flash to the Default values:

  - Cell0 R_a flag: addr = 0x4100, data = 0xFF55 - Cell impedance never updated; Table being used;
  - Cell1 R_a flag: addr = 0x4140, data = 0xFF55 - Cell impedance never updated; Table being used;
  - xCell0 R_a flag: addr = 0x4180, data = 0xFFFF - Cell impedance never updated; Table never used;
  - xCell0 R_a flag: addr = 0x41C0, data = 0xFFFF - Cell impedance never updated; Table never used;

  @see DF_ADDR::CELL0_RA_FLAG
  @see DF_ADDR::CELL1_RA_FLAG
  @see DF_ADDR::X_CELL0_RA_FLAG
  @see DF_ADDR::X_CELL1_RA_FLAG
*/
void dfResetRaTableFlags() {
  const word TABLE_USED_NOT_UPDATED = 0xFF55;
  const word TABLE_NOT_USED_NOT_UPDATED = 0xFFFF;

  dfWriteWord(DF_ADDR::CELL0_RA_FLAG, TABLE_USED_NOT_UPDATED);  // Cell0 R_a flag
  dfWriteWord(DF_ADDR::CELL1_RA_FLAG, TABLE_USED_NOT_UPDATED);  // Cell1 R_a flag
  dfWriteWord(DF_ADDR::X_CELL0_RA_FLAG, TABLE_NOT_USED_NOT_UPDATED);  // xCell0 R_a flag
  dfWriteWord(DF_ADDR::X_CELL1_RA_FLAG, TABLE_NOT_USED_NOT_UPDATED);  // xCell0 R_a flag
}

/**
  @brief Read and print all data from the Data Flash.

  @see DF_ADDR::MIN
  @see DF_ADDR::MAX
  @see BlockProtocol::PAYLOAD_MAX_SIZE
*/
void dfReadAllData() {
  static const byte DATA_SIZE = BlockProtocol::PAYLOAD_MAX_SIZE;  // 32
  byte data[DATA_SIZE];

  // addr from 0x4000 till 0x5FE0 with step 0x20 (32)
  for (word addr = DF_ADDR::MIN; addr <= (DF_ADDR::MAX - DATA_SIZE + 1); addr += DATA_SIZE) {
    memset(data, 0, DATA_SIZE);
    dfReadBytes(addr, data, DATA_SIZE);
    printWordHex(addr);
    PGM_PRINT(": ");
    printBytesHex(data, DATA_SIZE);
  }
}

/**
  @brief Print data from R_a table.

  Cell 0, Cell 1, xCell 0, xCell 1:
  - R_a flag
  - 15 rows

  @see DF_ADDR::CELL0_RA_FLAG
  @see DF_ADDR::CELL1_RA_FLAG
  @see DF_ADDR::X_CELL0_RA_FLAG
  @see DF_ADDR::X_CELL1_RA_FLAG
*/
void dfPrintRaTable() {
  static const int RA_SIZE = 4;
  static const int RA_ROW_SIZE = 32;
  static const word RA_TABLE[RA_SIZE] = {
    DF_ADDR::CELL0_RA_FLAG,
    DF_ADDR::CELL1_RA_FLAG,
    DF_ADDR::X_CELL0_RA_FLAG,
    DF_ADDR::X_CELL1_RA_FLAG
  };

  byte buf[RA_ROW_SIZE];
  for (int i = 0; i < RA_SIZE; i++) {
    const word addr = RA_TABLE[i];

    memset(buf, 0, sizeof(buf));
    dfReadBytes(addr, buf, sizeof(buf));

    printWordHex(addr);
    PGM_PRINTLN(": ");
    printBytesHex(buf, sizeof(buf));
  }
}

/**
  @brief Advanced Charge Algorithm; Termination Config; 0x4693; Charge Term Taper Current; I2
  @see DF_ADDR::CHARGE_TERM_TAPER_CURRENT
*/
int dfReadTaperCurrent() {
  return dfReadI2(DF_ADDR::CHARGE_TERM_TAPER_CURRENT);
}

/**
  @brief Advanced Charge Algorithm; Termination Config; 0x4693; Charge Term Taper Current; I2
  @see DF_ADDR::CHARGE_TERM_TAPER_CURRENT
*/
void dfWriteTaperCurrent(int taperCurrent) {
  dfWriteI2(DF_ADDR::CHARGE_TERM_TAPER_CURRENT, taperCurrent);
}

/**
  @brief Settings; Configuration; FET Options; H1

  Bit 5: CHGFET - FET action on valid charge termination
  - 0 = FET active
  - 1 = Charging or precharging disabled, FET off

  @see DF_ADDR::FET_OPTIONS
  @see FetOptionsFlags::CHGFET()
*/
void dfWriteFetOptionsCHGFET(bool chgFetBitValue) {
  byte fetOptions = dfReadByte(DF_ADDR::FET_OPTIONS);
  bitWrite(fetOptions, FetOptionsFlags::CHGFET().n, chgFetBitValue);
  dfWriteByte(DF_ADDR::FET_OPTIONS, fetOptions);
}

/**
  @brief Settings; Configuration; 0x4632; SOC Flag Config A; H2

  <pre>
  Bit 0: TDSETV—Enables the TD flag set by the cell voltage threshold
    0 = Disabled (default)
    1 = Enabled
  Bit 1: TDCLEARV—Enables the TD flag clear by cell voltage threshold
    0 = Disabled (default)
    1 = Enabled
  Bit 2: TDSETRSOC—Enables the TD flag set by RSOC threshold
    0 = Disabled
    1 = Enabled (default)
  Bit 3: TDCLEARRSOC—Enables the TD flag cleared by the RSOC threshold
    0 = Disabled
    1 = Enabled (default)
  Bit 4: TCSETV—Enables the TC flag set by cell voltage threshold
    0 = Disabled (default)
    1 = Enabled
  Bit 5: TCCLEARV—Enables the TC flag clear by cell voltage threshold
    0 = Disabled (default)
    1 = Enabled
  Bit 6: TCSETRSOC—Enables the TC flag set by the RSOC threshold
    0 = Disabled (default)
    1 = Enabled
  Bit 7: TCCLEARRSOC—Enables the TC flag cleared by the RSOC threshold
    0 = Disabled
    1 = Enabled (default)
  Bit 8: Reserved
  Bit 9: Reserved
  Bit 10: FCSETVCT—Enables the FC flag set by primary charge termination
    0 = Disabled
    1 = Enabled (default)
  Bit 11: TCSETVCT—Enables the TC flag set by primary charge termination
    0 = Disabled
    1 = Enabled (default)
  Bit 15: 12: Reserved
  </pre>

  @returns
  - Default: 0x0C8C;
  - JBL: 0001 0011 1000 0011;

  @see DF_ADDR::SOC_FLAG_CONFIG_A
  @see SOCFlagConfigAFlags
*/
word dfReadSocFlagConfigA() {
  const word addr = DF_ADDR::SOC_FLAG_CONFIG_A;
  const word retval = dfReadWord(addr);
  if (!SILENCE) {
    printWordHex(PSTR("\n=== Data Flash [SOC Flag Config A]"), addr);
    printWordBin(retval, true);
    printFlag(retval, SOCFlagConfigAFlags::TDSETV());  // Bit 0: TDSETV - Enables the TD flag set by the cell voltage threshold
    printFlag(retval, SOCFlagConfigAFlags::TDCLEARV());  // Bit 1: TDCLEARV - Enables the TD flag clear by cell voltage threshold
    printFlag(retval, SOCFlagConfigAFlags::TDSETRSOC());  // Bit 2: TDSETRSOC - Enables the TD flag set by RSOC threshold
    printFlag(retval, SOCFlagConfigAFlags::TDCLEARRSOC());  // Bit 3: TDCLEARRSOC - Enables the TD flag cleared by the RSOC threshold
    printFlag(retval, SOCFlagConfigAFlags::TCSETV());  // Bit 4: TCSETV - Enables the TC flag set by cell voltage threshold
    printFlag(retval, SOCFlagConfigAFlags::TCCLEARV());  // Bit 5: TCCLEARV - Enables the TC flag clear by cell voltage threshold
    printFlag(retval, SOCFlagConfigAFlags::TCSETRSOC());  // Bit 6: TCSETRSOC - Enables the TC flag set by the RSOC threshold
    printFlag(retval, SOCFlagConfigAFlags::TCCLEARRSOC());  // Bit 7: TCCLEARRSOC - Enables the TC flag cleared by the RSOC threshold
    printFlag(retval, SOCFlagConfigAFlags::FCSETVCT());  // Bit 10: FCSETVCT - Enables the FC flag set by primary charge termination
    printFlag(retval, SOCFlagConfigAFlags::TCSETVCT());  // Bit 11: TCSETVCT - Enables the TC flag set by primary charge termination
  }
  return retval;
}

/**
  @brief Settings; Configuration; 0x4632; SOC Flag Config A; H2

  <pre>
  Bit 0: TDSETV—Enables the TD flag set by the cell voltage threshold
    0 = Disabled (default)
    1 = Enabled
  Bit 1: TDCLEARV—Enables the TD flag clear by cell voltage threshold
    0 = Disabled (default)
    1 = Enabled
  Bit 2: TDSETRSOC—Enables the TD flag set by RSOC threshold
    0 = Disabled
    1 = Enabled (default)
  Bit 3: TDCLEARRSOC—Enables the TD flag cleared by the RSOC threshold
    0 = Disabled
    1 = Enabled (default)
  Bit 4: TCSETV—Enables the TC flag set by cell voltage threshold
    0 = Disabled (default)
    1 = Enabled
  Bit 5: TCCLEARV—Enables the TC flag clear by cell voltage threshold
    0 = Disabled (default)
    1 = Enabled
  Bit 6: TCSETRSOC—Enables the TC flag set by the RSOC threshold
    0 = Disabled (default)
    1 = Enabled
  Bit 7: TCCLEARRSOC—Enables the TC flag cleared by the RSOC threshold
    0 = Disabled
    1 = Enabled (default)
  Bit 8: Reserved
  Bit 9: Reserved
  Bit 10: FCSETVCT—Enables the FC flag set by primary charge termination
    0 = Disabled
    1 = Enabled (default)
  Bit 11: TCSETVCT—Enables the TC flag set by primary charge termination
    0 = Disabled
    1 = Enabled (default)
  Bit 15: 12: Reserved
  </pre>

  @returns
  - Default: 0x0C8C;
  - JBL: 0001 0011 1000 0011;

  @see DF_ADDR::SOC_FLAG_CONFIG_A
*/
void dfWriteSocFlagConfigA(word socFlagConfigA) {
  dfWriteWord(DF_ADDR::SOC_FLAG_CONFIG_A, socFlagConfigA);
}

/**
  @brief Gas Gauging; TC; 0x464B; Set % RSOC Threshold
  @see DF_ADDR::TC_SET_RSOC_THRESHOLD
*/
byte dfReadTcSetRsocThreshold() {
  return dfReadByte(DF_ADDR::TC_SET_RSOC_THRESHOLD);
}

/**
  @brief Gas Gauging; TC; 0x464B; Set % RSOC Threshold
  @see DF_ADDR::TC_SET_RSOC_THRESHOLD
*/
void dfWriteTcSetRsocThreshold(byte setThreshold) {
  dfWriteByte(DF_ADDR::TC_SET_RSOC_THRESHOLD, setThreshold);
}

/**
  @brief Gas Gauging; TC; 0x464C; Clear % RSOC Threshold
  @see DF_ADDR::TC_CLEAR_RSOC_THRESHOLD
*/
byte dfReadTcClearRsocThreshold() {
  return dfReadByte(DF_ADDR::TC_CLEAR_RSOC_THRESHOLD);
}

/**
  @brief Gas Gauging; TC; 0x464C; Clear % RSOC Threshold
  @see DF_ADDR::TC_CLEAR_RSOC_THRESHOLD
*/
void dfWriteTcClearRsocThreshold(byte clearThreshold) {
  dfWriteByte(DF_ADDR::TC_CLEAR_RSOC_THRESHOLD, clearThreshold);
}

/**
  @brief Gas Gauging; State; 0x4206; Qmax Cell 1; I2
  @see DF_ADDR::Q_MAX_CELL_1
*/
int dfReadQmaxCell1() {
  return dfReadI2(DF_ADDR::Q_MAX_CELL_1);
}

/**
  @brief Gas Gauging; State; 0x4208; Qmax Cell 2; I2
  @see DF_ADDR::Q_MAX_CELL_2
*/
int dfReadQmaxCell2() {
  return dfReadI2(DF_ADDR::Q_MAX_CELL_2);
}

/**
  @brief Gas Gauging; State; 0x420A; Qmax Pack; I2
  @see DF_ADDR::Q_MAX_PACK
*/
int dfReadQmaxPack() {
  return dfReadI2(DF_ADDR::Q_MAX_PACK);
}

/**
  @brief Qmax Pack = min(Qmax Cell 1, Qmax Cell 2)

  - Gas Gauging; State; 0x4206; Qmax Cell 1; I2
  - Gas Gauging; State; 0x4208; Qmax Cell 2; I2
  - Gas Gauging; State; 0x420A; Qmax Pack; I2

  @see DF_ADDR::Q_MAX_CELL_1
  @see DF_ADDR::Q_MAX_CELL_2
  @see DF_ADDR::Q_MAX_PACK
*/
void dfWriteQmax(word qMaxCell1, word qMaxCell2) {
  const word qMaxPack = min(qMaxCell1, qMaxCell2);  // Qmax Pack

  dfWriteWord(DF_ADDR::Q_MAX_CELL_1, qMaxCell1);  // Qmax Cell 1
  dfWriteWord(DF_ADDR::Q_MAX_CELL_2, qMaxCell2);  // Qmax Cell 2
  dfWriteWord(DF_ADDR::Q_MAX_PACK, qMaxPack);  // Qmax Pack
}

/**
  @brief Gas Gauging; State; 0x4240; Cycle Count; U2
  @see DF_ADDR::GAS_GAUGING_CYCLE_COUNT
*/
word dfReadCycleCount() {
  return dfReadU2(DF_ADDR::GAS_GAUGING_CYCLE_COUNT);
}

/**
  @brief Gas Gauging; State; 0x4240; Cycle Count; U2
  @see DF_ADDR::GAS_GAUGING_CYCLE_COUNT
*/
void dfWriteCycleCount(word cycleCount) {
  dfWriteU2(DF_ADDR::GAS_GAUGING_CYCLE_COUNT, cycleCount);
}
