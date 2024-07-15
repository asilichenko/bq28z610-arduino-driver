/**
  @file data_flash_access.h

  @brief 12.2.45 Data Flash Access() 0x4000–0x5FFF headers

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

#pragma once

#include <Arduino.h>

#include "globals.h"
#include "utils.h"
#include "alt_manufacturer_access.h"
#include "service.h"

/**
  @brief Data Flash addresses
*/
class DF_ADDR {
  public:
    static const word MIN = 0x4000;  ///< Minimum Data Flash address.
    static const word MAX = 0x5FFF;  ///< Maximum Data Flash address.

    /**
      @retval JBL: XTREME2
    */
    static const word MANUFACTURER_NAME = 0x406B;  ///< I2C Configuration; Data; Manufacturer Name; S21
    /**
      @retval JBL: ID1019-A-M26-28z610
    */
    static const word DEVICE_NAME = 0x4080;  ///< I2C Configuration; Data; Device Name; S21
    /**
      @retval JBL: 1352 - this is ID of the type for the Li-Ion battery
    */
    static const word DEVICE_CHEMISTRY = 0x4095;  ///< I2C Configuration; Data; Device Chemistry; S5

    static const word GAS_GAUGING_CYCLE_COUNT = 0x4240;  ///< Gas Gauging; State; Cycle Count; U2
    /**
      @retval Default: 0x0E
    */
    static const word GAS_GAUGING_UPDATE_STATUS = 0x420E;  ///< Gas Gauging; State; Update Status; H1

    static const word FET_OPTIONS = 0x4600;  ///< Settings; Configuration; FET Options; H1

    /**
      @retval JBL: 5100 (mAh)
    */
    static const word DESIGN_CAPACITY_MAH = 0x462A;  ///< Gas Gauging; Design; Design Capacity mAh; I2
    /**
      @retval JBL: 3672 (cWh)
    */
    static const word DESIGN_CAPACITY_CWH = 0x462C;  ///< Gas Gauging; Design; Design Capacity cWh; I2

    static const word SOC_FLAG_CONFIG_A = 0x4632;  ///< Settings; Configuration; 0x4632; SOC Flag Config A; H2

    /**
      @retval Default: 100
    */
    static const word TC_SET_RSOC_THRESHOLD = 0x464B;  ///< Gas Gauging; TC; Set % RSOC Threshold; U1
    /**
      @retval Default: 95
    */
    static const word TC_CLEAR_RSOC_THRESHOLD = 0x464C;  ///< Gas Gauging; TC; Clear % RSOC Threshold; U1

    static const word DA_CONFIGURATION = 0x469B;  ///< Settings; Configuration; DA Configuration; H1

    /**
      @retval JBL 330 (mA)
    */
    static const word CHARGE_TERM_TAPER_CURRENT = 0x4693;  ///< Advanced Charge Algorithm; Termination Config; Charge Term Taper Current; I2

    static const word OCC_THRESHOLD = 0x46C9;  ///< 13.10.3 OCC- Overcurrent In Charge: Protections; OCC; 0x46C9; Threshold; I2

    /**
      @retval JBL: 46.0 °C
    */
    static const word OTC_THRESHOLD = 0x46D8;  ///< 13.10.8 OTC - Overtemperature in Charge: Protections; OTC; 0x46D8; Threshold; I2
    static const word OTC_RECOVERY = 0x46DB;   ///< 13.10.8 OTC - Overtemperature in Charge: Protections; OTC; 0x46DB; Recovery; I2

    /**
      @retval JBL 4965
    */
    static const word Q_MAX_CELL_1 = 0x4206;  ///< Gas Gauging; State; Qmax Cell 1; I2
    /**
      @retval JBL 4931
    */
    static const word Q_MAX_CELL_2 = 0x4208;  ///< Gas Gauging; State; Qmax Cell 2; I2
    /**
      @retval JBL 4931
    */
    static const word Q_MAX_PACK = 0x420A;   ///< Gas Gauging; State; Qmax Pack; I2

    /**
      Ra Table; R_a0; 0x4100; Cell0 R_a flag; H2

      High Byte:
      - 0x00 - Cell impedance and QMax updated
      - 0x05 - RELAX mode and QMax update in progress
      - 0x55 - DISCHARGE mode and cell impedance updated
      - 0xFF - Cell impedance never updated

      Low Byte:
      - 0x00 - Table not used and QMax updated
      - 0x55 - Table being used
      - 0xFF - Table never used, neither QMax nor cell impedance updated

      Following it are 15 two-byte (I2) values of the corresponding row of the table.

      @retval Default 0xFF55
      @see 6.4.5 Ra Table Initial Values
    */
    static const word CELL0_RA_FLAG = 0x4100;
    /**
      High Byte:
      - 0x00 - Cell impedance and QMax updated
      - 0x05 - RELAX mode and QMax update in progress
      - 0x55 - DISCHARGE mode and cell impedance updated
      - 0xFF - Cell impedance never updated

      Low Byte:
      - 0x00 - Table not used and QMax updated
      - 0x55 - Table being used
      - 0xFF - Table never used, neither QMax nor cell impedance updated

      Following it are 15 two-byte (I2) values of the corresponding row of the table.

      @retval Default 0xFF55
      @see 6.4.5 Ra Table Initial Values
    */
    static const word CELL1_RA_FLAG = 0x4140;  ///< Ra Table; R_a1; 0x4140; Cell1 R_a flag; H2
    /**
      The initial x-values are a copy of the non-x data set.
      Two sets of Ra tables are used alternatively when gauging
      is enabled to prevent wearing out the data flash.

      High Byte:
      - 0x00 - Cell impedance and QMax updated
      - 0x05 - RELAX mode and QMax update in progress
      - 0x55 - DISCHARGE mode and cell impedance updated
      - 0xFF - Cell impedance never updated

      Low Byte:
      - 0x00 - Table not used and QMax updated
      - 0x55 - Table being used
      - 0xFF - Table never used, neither QMax nor cell impedance updated

      Following it are 15 two-byte (I2) values of the corresponding row of the table.

      @retval Default 0xFFFF
      @see 6.4.5 Ra Table Initial Values
    */
    static const word X_CELL0_RA_FLAG = 0x4180;  ///< Ra Table; R_a0x; 0x4180; xCell0 R_a flag; H2
    /**
      The initial x-values are a copy of the non-x data set.
      Two sets of Ra tables are used alternatively when gauging
      is enabled to prevent wearing out the data flash.

      High Byte:
      - 0x00 - Cell impedance and QMax updated
      - 0x05 - RELAX mode and QMax update in progress
      - 0x55 - DISCHARGE mode and cell impedance updated
      - 0xFF - Cell impedance never updated

      Low Byte:
      - 0x00 - Table not used and QMax updated
      - 0x55 - Table being used
      - 0xFF - Table never used, neither QMax nor cell impedance updated

      Following it are 15 two-byte (I2) values of the corresponding row of the table.

      @retval Default 0xFFFF
      @see 6.4.5 Ra Table Initial Values
    */
    static const word X_CELL1_RA_FLAG = 0x41C0;  ///< Ra Table; R_a1x; 0x41C0; xCell1 R_a flag; H2
};

/**
  @brief Read array of bytes from the Data Flash by address.

  Address should be fit into Data Flash addresses region, otherwise error message will be printed.

  Requested length should be in the range [1; 32]

  @see AltManufacturerAccess()
  @see DF_ADDR::MIN
  @see DF_ADDR::MAX
  @see BlockProtocol
*/
void dfReadBytes(word addr, byte *retval, int len);

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
void dfWriteBytes(word addr, byte *data, int len);

/**
  @see dfReadBytes()
*/
byte dfReadByte(word addr);

/**
  @see dfReadByte()
*/
byte dfReadU1(word addr);

/**
  @see dfWriteBytes()
*/
void dfWriteByte(word addr, byte value);

/**
  @brief Read a word value (two bytes) from the Data Flash by address.

  Result is in the Normal Order.

  @param addr - two-byte address
  @returns two byte in normal order: 0x4321

  @see dfReadBytes()
  @see composeWord()
*/
word dfReadWord(word addr);

/**
  @brief Write a word value (two bytes) to the Data Flash by address.

  Order of bytes should be Normal.

  @see dfWriteBytes()
*/
void dfWriteWord(word addr, word value);

/**
  @brief Read Two-byte Integer value from the Data Flash by address.
  @see dfReadWord()
*/
int dfReadI2(word addr);

/**
  @brief Write int value to the Data Flash by address.
  @see dfWriteWord()
*/
void dfWriteI2(word addr, int value);

/**
  @brief Read Two-byte unsigned integer value from the Data Flash by address.
  @see dfReadWord()
*/
word dfReadU2(word addr);

/**
  @brief Write unsigned int value to the Data Flash by address.
  @see dfWriteWord()
*/
void dfWriteU2(word addr, word value);

/**
  @brief Read String value from the Data Flash by address.
  @see AltManufacturerAccess()
*/
String dfReadString(word addr);

/**
  @brief Read the Device Name from the Data Flash.

  I2C Configuration; Data; 0x4080; Device Name; S21

  @see DF_ADDR::DEVICE_NAME
*/
String dfDeviceName();

/**
  @brief Read the Design Capacity in mAh from the Data Flash.

  Gas Gauging; Design; 0x462A; Design Capacity mAh; I2

  Is used for LOAD mode = Constant Current.

  @see DF_ADDR::DESIGN_CAPACITY_MAH
*/
int dfReadDesignCapacityMah();

/**
  @brief Write Design Capacity in mAh to the Data Flash.

  Gas Gauging; Design; 0x462A; Design Capacity mAh; I2
  - JBL: 5100

  @see DF_ADDR::DESIGN_CAPACITY_MAH
*/
void dfWriteDesignCapacityMah(int designCapacityMah);

/**
  @brief Read the Design Energy in cWh from the Data Flash.

  Gas Gauging; Design; 0x462C; Design Capacity cWh; I2

  Is used for LOAD mode = Constant Power.

  @see DF_ADDR::DESIGN_CAPACITY_CWH
*/
int dfReadDesignCapacityCwh();

/**
  @brief Write Design Energy in cWh to the Data Flash.

  Gas Gauging; Design; 0x462C; Design Capacity cWh; I2
  - JBL: 3672

  Design Energy = Nominal Voltage * Capacity
  7.2 V * 5100 mAh = 36'720 mWh = 3672 cWh

  @see DF_ADDR::DESIGN_CAPACITY_CWH
*/
void dfWriteDesignCapacityCwh(int designCapacityCwh);

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
byte dfReadFetOptions();

/**
  @brief Settings; Configuration; FET Options; H1

  Bit 5: CHGFET - FET action on valid charge termination
  - 0 = FET active
  - 1 = Charging or precharging disabled, FET off

  @see DF_ADDR::FET_OPTIONS
  @see FetOptionsFlags::CHGFET()
*/
void dfWriteFetOptionsCHGFET(bool chgFetBitValue);

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
byte dfReadDaConfiguration();

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
byte dfReadGasGaugingUpdateStatus();

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
void dfWriteGasGaugingUpdateStatus(byte updateStatus);

/**
  @brief Read and print all data from the Data Flash.

  @see DF_ADDR::MIN
  @see DF_ADDR::MAX
  @see BlockProtocol::PAYLOAD_MAX_SIZE
*/
void dfReadAllData();

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
void dfPrintRaTable();

/**
  @brief Advanced Charge Algorithm; Termination Config; 0x4693; Charge Term Taper Current; I2
  @see DF_ADDR::CHARGE_TERM_TAPER_CURRENT
*/
int dfReadTaperCurrent();

/**
  @brief Advanced Charge Algorithm; Termination Config; 0x4693; Charge Term Taper Current; I2
  @see DF_ADDR::CHARGE_TERM_TAPER_CURRENT
*/
void dfWriteTaperCurrent(int taperCurrent);

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
void dfResetRaTableFlags();

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
word dfReadSocFlagConfigA();

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
void dfWriteSocFlagConfigA(word socFlagConfigA);

/**
  @brief Gas Gauging; TC; 0x464B; Set % RSOC Threshold
  @see DF_ADDR::TC_SET_RSOC_THRESHOLD
*/
byte dfReadTcSetRsocThreshold();

/**
  @brief Gas Gauging; TC; 0x464B; Set % RSOC Threshold
  @see DF_ADDR::TC_SET_RSOC_THRESHOLD
*/
void dfWriteTcSetRsocThreshold(byte setThreshold);

/**
  @brief Gas Gauging; TC; 0x464C; Clear % RSOC Threshold
  @see DF_ADDR::TC_CLEAR_RSOC_THRESHOLD
*/
byte dfReadTcClearRsocThreshold();

/**
  @brief Gas Gauging; TC; 0x464C; Clear % RSOC Threshold
  @see DF_ADDR::TC_CLEAR_RSOC_THRESHOLD
*/
void dfWriteTcClearRsocThreshold(byte clearThreshold);

/**
  @brief Gas Gauging; State; 0x4206; Qmax Cell 1; I2
  @see DF_ADDR::Q_MAX_CELL_1
*/
int dfReadQmaxCell1();

/**
  @brief Gas Gauging; State; 0x4208; Qmax Cell 2; I2
  @see DF_ADDR::Q_MAX_CELL_2
*/
int dfReadQmaxCell2();

/**
  @brief Gas Gauging; State; 0x420A; Qmax Pack; I2
  @see DF_ADDR::Q_MAX_PACK
*/
int dfReadQmaxPack();

/**
  @brief Qmax Pack = min(Qmax Cell 1, Qmax Cell 2)

  - Gas Gauging; State; 0x4206; Qmax Cell 1; I2
  - Gas Gauging; State; 0x4208; Qmax Cell 2; I2
  - Gas Gauging; State; 0x420A; Qmax Pack; I2

  @see DF_ADDR::Q_MAX_CELL_1
  @see DF_ADDR::Q_MAX_CELL_2
  @see DF_ADDR::Q_MAX_PACK
*/
void dfWriteQmax(word qMaxCell0, word qMaxCell1);

/**
  @brief Gas Gauging; State; 0x4240; Cycle Count; U2
  @see DF_ADDR::GAS_GAUGING_CYCLE_COUNT
*/
word dfReadCycleCount();

/**
  @brief Gas Gauging; State; 0x4240; Cycle Count; U2
  @see DF_ADDR::GAS_GAUGING_CYCLE_COUNT
*/
void dfWriteCycleCount(word cycleCount);
