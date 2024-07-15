/**
  @file alt_manufacturer_access.h

  @brief 12.2 0x3E, 0x3F AltManufacturerAccess() headers

  The host controller communicates with the gauge through gauge commands.
  A gauge command is the equivalent to a register. For example, reading the state of charge is
  accomplished through the StateOfCharge() gauge command, which has the command codes 0x1C and
  0x1D. So if the interface is I2C, reading from device register 0x1C and 0x1D returns the current state of
  charge of the battery as calculated by the gauge.

  AltManufacturerAccess() provides a method of reading and writing data in the Manufacturer Access System
  (MAC). The MAC command is sent via AltManufacturerAccess() by a block protocol. The result is returned on
  AltManufacturerAccess() via a block read.

  Commands are set by writing to registers 0x00/0x01. On valid word access, the MAC command state is set, and
  commands 0x3E and 0x3F are used for MAC commands. These new addresses work the same as 0x00 and
  0x01, but are primarily intended for block writes and reads.

  <pre>
  BQ28Z610
  Technical Reference Manual
  (c) Texas Instruments
  Literature Number: SLUUA65E
  </pre>

  @see https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf


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

#include "globals.h"
#include "utils.h"

/**
  @brief 12.2 0x3E, 0x3F AltManufacturerAccess

  Provides a method of reading and writing data in the Manufacturer Access System (MAC).
  The MAC command is sent via AltManufacturerAccess by a block protocol.
  The result is returned on AltManufacturerAccess via a block read.

  Send subcommand to 0x3E AltManufacturerAccess and request block of data.

  @returns whether the request was successful
*/
bool AltManufacturerAccess(const word MACSubcmd, byte *retval, byte *len);

/**
  @brief 12.2 0x3E/0x3F AltManufacturerAccess

  Send subcommand to 0x3E AltManufacturerAccess.
*/
void AltManufacturerAccess(const word MACSubcmd);

/**
  @brief 12.2.1 AltManufacturerAccess() 0x0001 Device Type

  The device can be checked for the IC part number.

  @returns the IC part number on subsequent read on MACData() in the following format:
  @returns aaAA
  @returns
  - AAaa: Device type

  @retval JBL: 2610
*/
word DeviceType();

/**
  @brief 12.2.2 AltManufacturerAccess() 0x0002 Firmware Version.

  The device can be checked for the firmware version of the IC

  The firmware revision on MACData() in the following format:

  ddDDvvVVbbBBTTzzZZRREE

  - ddDD: Device Number
  - vvVV: Version
  - bbBB: build number
  - ttTT: Firmware type
  - zzZZ: Impedance Track Version
  - RR: Reserved
  - EE: Reserved

  @retval JBL:
  - Device Number: 0x2610
  - Version: 0x0017
  - Build number: 0x0016
  - Firmware type: 0x00
  - Impedance Track Version: 0x0385
*/
void FirmwareVersion();

/**
  @brief 12.2.3 AltManufacturerAccess() 0x0003 Hardware Version.

  The device can be checked for the hardware version of the IC.

  @returns the hardware revision on subsequent read on MACData()
*/
word HardwareVersion();

/**
  @brief 12.2.6 AltManufacturerAccess() 0x0006 Chemical ID.
  @returns the chemical ID of the OCV tables used in the gauging algorithm.
*/
word ChemicalID();

/**
  @brief 12.2.12 AltManufacturerAccess() 0x0012 Device Reset

  This command resets the device.

  @warning [!] Not Available in SEALED Mode
*/
void DeviceReset();

/**
  @brief 12.2.13 AltManufacturerAccess() 0x001F CHG FET

  This command turns on/off CHG FET drive function to ease testing during manufacturing.

  @warning [!] This toggling command will only set the RAM data,
               meaning the conditions set by the command will be cleared
               if a reset or seal is issued to the gauge.

  @warning [!] This command turns on the Charge FET only if DSG FET = 1.

  @warning [!] Not Available in SEALED Mode

  @see FETControl()
*/
void ChargeFET();

/**
  @brief 12.2.14 AltManufacturerAccess() 0x0020 DSG FET

  This command turns on/off DSG FET drive function to ease testing during manufacturing.

  @warning [!] This toggling command will only set the RAM data,
               meaning the conditions set by the command will be cleared
               if a reset or seal is issued to the gauge.

  @warning [!] Not Available in SEALED Mode

  @see 12.2.16 AltManufacturerAccess() 0x0022 FETControl()
*/
void DischargeFET();

/**
  @brief 12.2.15 AltManufacturerAccess() 0x0021 Gauging

  This command enables or disables the gauging function to ease testing during manufacturing.

  @see AltManufacturerCommands
*/
void Gauging();

/**
  @brief 12.2.16 AltManufacturerAccess() 0x0022 FET Control

  This command disables/enables control of the CHG, DSG, and PCHG FETs by the firmware.

  - Disable: ManufacturingStatus[FET_EN] = 1 -> 0, CHG and DSG FETs are disabled and remain OFF.
  - Enable:  ManufacturingStatus[FET_EN] = 0 -> 1, CHG and DSG FETs are controlled by the firmware

  @warning [!] Not Available in SEALED Mode
  @see ManufacturingStatus()
  @see ManufacturingStatusFlags
*/
void FETControl();

/**
  @brief 12.2.19 AltManufacturerAccess() 0x0028 Lifetime Data Reset

  This command resets Lifetime data in data flash for ease of manufacturing.

  Clears Lifetime Data in DF:

  - 0x4280: (I2) [Lifetimes / Voltage / Cell 1 Max Voltage] = 0
  - 0x4282: (I2) [Lifetimes / Voltage / Cell 2 Max Voltage] = 0
  - 0x4284: (I2) [Lifetimes / Current / Max Charge Current] = 0
  - 0x4286: (I2) [Lifetimes / Current / Max Discharge Current] = 0
  - 0x4288: (I1) [Lifetimes / Temperature / Max Temp Cell] = -128
  - 0x4289: (I1) [Lifetimes / Temperature / Min Temp Cell] = 127

  @warning [!] Not Available in SEALED Mode.
*/
void LifetimeDataReset();

/**
  @brief 12.2.20 AltManufacturerAccess() 0x0029 Permanent Fail Data Reset

  This command resets PF data in data flash for ease of manufacturing.

  @warning [!] Not Available in SEALED Mode.
*/
void PermanentFailureDataReset();

/**
  @brief 12.2.22 AltManufacturerAccess() 0x0030 Seal Device

  This command seals the device for the field, disabling certain commands and access to DF.

  @see unsealDevice()
  @see 9.5.2 SEALED to UNSEALED
*/
void SealDevice();

/**
  @brief 12.2.26 AltManufacturerAccess() 0x0050 SafetyAlert
  @returns the SafetyAlert flags on AltManufacturerAccess() or MACData().
*/
u32 SafetyAlert();

/**
  @brief 12.2.27 AltManufacturerAccess() 0x0051 SafetyStatus
  @returns the SafetyStatus flags on MACData().
  @see SafetyStatusFlags
*/
u32 SafetyStatus();

/**
  @brief 12.2.28 AltManufacturerAccess() 0x0052 PFAlert
  @returns the PFAlert flags on MACData().
*/
u32 PFAlert();

/**
  @brief 12.2.29 AltManufacturerAccess() 0x0053 PFStatus
  @returns the PFStatus flags on MACData().
  @see PFStatusFlags
*/
u32 PFStatus();

/**
  @brief 12.2.30 AltManufacturerAccess() 0x0054 OperationStatus
  @returns the OperationStatus flags on MACData()
  @see OperationStatusFlags
*/
u32 OperationStatus();

/**
  @brief 12.2.31 AltManufacturerAccess() 0x0055 ChargingStatus
  @returns the ChargingStatus flags on MACData().
  @see ChargingStatusFlags
*/
word ChargingStatus();

/**
  @brief 12.2.32 AltManufacturerAccess() 0x0056 GaugingStatus
  @returns the GaugingStatus flags on MACData().
  @see GaugingStatusFlags
*/
u32 GaugingStatus();

/**
  @brief 12.2.33 AltManufacturerAccess() 0x0057 ManufacturingStatus
  @returns the ManufacturingStatus flags on MACData().
  @see ManufacturingStatusFlags
*/
word ManufacturingStatus();

/**
  @brief 12.2.37 AltManufacturerAccess() 0x0071 DAStatus1

  @returns 32 bytes of data on MACData() in the following format:
  @returns aaAAbbBBccCCddDDeeEEffFFggGGhhHHiiIIjjJJkkKKllLLmmMMnnNNooOOppPP
  @returns
  - AAaa: Cell Voltage 1
  - BBbb: Cell Voltage 2
  - CCcc:
  - DDdd:
  - EEee: BAT Voltage. Voltage at the VC2 (BAT) terminal
  - FFff: PACK Voltage
  - GGgg: Cell Current 1. Simultaneous current measured during Cell Voltage1 measurement
  - HHhh: Cell Current 2. Simultaneous current measured during Cell Voltage2 measurement
  - IIii:
  - JJjj:
  - KKkk: Cell Power 1. Calculated using Cell Voltage1 and Cell Current 1 data
  - LLll: Cell Power 2. Calculated using Cell Voltage2 and Cell Current 2 data
  - MMmm:
  - NNnn:
  - OOoo: Power calculated by Voltage() × Current()
  - PPpp: Average Power. Calculated by Voltage() × AverageCurrent()
*/
void DAStatus1(byte *retval);

/**
  @brief 12.2.37 AltManufacturerAccess() 0x0071 DAStatus1

  Print result of DAStatus1.
*/
void DAStatus1();

/**
  @brief 12.2.39 AltManufacturerAccess() 0x0073 ITStatus1

  @returns 24 bytes of IT data values on MACData() in the following format:
  @returns aaAAbbBBccCCddDDeeEEffFFggGGhhHHiiIIjjJJkkKKllLL
  @returns
  - AAaa: True Rem Q. True remaining capacity in mAh from IT
          simulation before any filtering or smoothing function. This value can
          be negative or higher than FCC.
  - BBbb: True Rem E. True remaining energy in cWh from IT simulation
          before any filtering or smoothing function. This value can be
          negative or higher than FCC.
  - CCcc: Initial Q. Initial capacity calculated from IT simulation
  - DDdd: Initial E. Initial energy calculated from IT simulation
  - EEee: TrueFullChgQ. True full charge capacity
  - FFff: TrueFullChgE. True full charge energy
  - GGgg: T_sim. Temperature during the last simulation run (in 0.1K)
  - HHhh: T_ambient. Current estimated ambient temperature used by
          the IT algorithm for thermal modeling
  - IIii: RaScale 0. Ra table scaling factor of Cell1
  - JJjj: RaScale 1. Ra table scaling factor of Cell2
  - KKkk: CompRes1. Last computed resistance for Cell1
  - LLll: CompRes2. Last computed resistance for Cell2
*/
void ITStatus1(byte *retval);

/**
  @brief 12.2.39 AltManufacturerAccess() 0x0073 ITStatus1

  Print result of ITStatus1.
*/
void ITStatus1();

/**
  @brief 12.2.40 AltManufacturerAccess() 0x0074 ITStatus2

  This command instructs the device to return Impedance Track related gauging information on MACData().

  @returns 24 bytes of IT data values on MACData() in the following format:
  @returns AA BB CC DD EE FF ggGGhhHH iiII jjJJ kkKK llLL mmMM nnNN ooOO
  @returns
  <pre>
  AA: Pack Grid. Active pack grid point (only valid in discharge)

  BB: LStatus—Learned status of resistance table
    Bit 3 | Bit 2 | Bit 1| Bit 0
    QMax  | ITEN  | CF1  | CF0

    CF1, CF0: QMax Status
      0,0 = Battery OK
      0,1 = QMax is first updated in learning cycle.
      1,0 = QMax and resistance table updated in learning cycle

    ITEN: IT enable
      0 = IT disabled
      1 = IT enabled

    QMax: QMax field updates
      0 = QMax is not updated in the field.
      1 = QMax is updated in the field.

    00: IT disabled
    04: Learning in process.
    05: QMax is first updated in learning cycle.
    06: Resistance table is updated.
    0E: Learning finished.

  CC: Cell Grid 1. Active grid point of Cell1
  DD: Cell Grid 2. Active grid point of Cell2
  EE: N/A. RSVD
  FF: N/A. RSVD
  HHhhGGgg: State Time. Time passed since last state change (Discharge, Charge, Rest)
  IIii: DOD0_1. Depth of discharge for Cell1
  JJjj: DOD0_2. Depth of discharge for Cell2
  KKkk: DOD0 Passed Q. Passed charge since DOD0
  LLll: DOD0 Passed Energy. Passed energy since the last DOD0 update
  MMmm: DOD0 Time. Time passed since the last DOD0 update
  NNnn: DODEOC_1. Cell 1 DOD@EOC
  OOoo: DODEOC_2. Cell 2 DOD@EOC
  </pre>
*/
void ITStatus2(byte *retval);

/**
  @brief 12.2.40 AltManufacturerAccess() 0x0074 ITStatus2

  Print result of ITStatus2.
*/
void ITStatus2();

/**
  @brief 12.2.41 AltManufacturerAccess() 0x0075 ITStatus3

  @returns 20 bytes of IT data values on MACData() in the following format:
  @returns aaAAbbBBccCCddDDeeEEffFFggGGhhHHIiiIIjjJJ
  @returns
  - AAaa: QMax 1. QMax of Cell 1
  - BBbb: QMax 2. QMax of Cell 2
  - CCcc: QMaxDOD0_1. Cell 1 DOD for Qmax
  - DDdd: QMaxDOD0_2. Cell 2 DOD for Qmax
  - EEee: QMaxPassedQ. Passed charge since DOD for Qmax recorded (mAh)
  - FFff: QMaxTime. Time since DOD for Qmax recorded (hour / 16 units)
  - GGgg: Tk. Thermal model “k”
  - HHhh: Ta. Thermal model “a”
  - IIii: RawDOD0_1. Cell 1 raw DOD0 measurement
  - JJjj: RawDOD0_2. Cell 2 raw DOD0 measurement
*/
void ITStatus3(byte *retval);

/**
  @brief 12.2.41 AltManufacturerAccess() 0x0075 ITStatus3

  Print result of ITStatus3.
*/
void ITStatus3();
