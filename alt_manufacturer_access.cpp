/**
  @file alt_manufacturer_access.cpp

  @brief 12.2 0x3E, 0x3F AltManufacturerAccess() implementation

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

#include "alt_manufacturer_access.h"

/**
  @brief 12.2 0x3E, 0x3F AltManufacturerAccess

  Provides a method of reading and writing data in the Manufacturer Access System (MAC).
  The MAC command is sent via AltManufacturerAccess by a block protocol.
  The result is returned on AltManufacturerAccess via a block read.

  Send subcommand to 0x3E AltManufacturerAccess and request block of data.

  @returns whether the request was successful
*/
bool AltManufacturerAccess(const word MACSubcmd, byte *retval, byte *len) {
  byte buf[BlockProtocol::RESPONSE_MAX_SIZE];
  memset(buf, 0, sizeof(buf));

  sendCommand(StdCommands::ALT_MANUFACTURER_ACCESS, MACSubcmd);

  delay(5);  // The delay required for the chip to process the request

  sendCommand(StdCommands::ALT_MANUFACTURER_ACCESS);
  const int count = requestBlock(buf);

  if (DEBUG) {
    PGM_PRINT("Obtained bytes: ");
    printBytesHex(buf, count);
  }

  const bool isDataValid = validate(buf);
  if (isDataValid) {
    const int _len = buf[BlockProtocol::LENGTH_INDEX] - BlockProtocol::SERVICE_SIZE;
    for (int i = 0; i < _len; i++) retval[i] = buf[BlockProtocol::DATA_INDEX + i];
    *len = _len;

    if (DEBUG) {
      PGM_PRINT("Data bytes: ");
      printBytesHex(retval, _len);
    }
  }
  return isDataValid;
}

/**
  @brief 12.2 0x3E/0x3F AltManufacturerAccess

  Send subcommand to 0x3E AltManufacturerAccess.
*/
void AltManufacturerAccess(const word MACSubcmd) {
  sendCommand(StdCommands::ALT_MANUFACTURER_ACCESS, MACSubcmd);
}

/**
  @brief 12.2.1 AltManufacturerAccess() 0x0001 Device Type

  The device can be checked for the IC part number.

  @returns the IC part number on subsequent read on MACData() in the following format:
  @returns aaAA
  @returns
  - AAaa: Device type

  @retval JBL: 2610
*/
word DeviceType() {
  if (!SILENCE) PGM_PRINT("=== 12.2.1 AltManufacturerAccess() 0x0001 Device Type: ");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::DEVICE_TYPE, buf, &len)) return 0;

  const word retval = composeWord(buf);
  if (!SILENCE) {
    if (DEBUG) printBytesHex(buf, len);
    printWordHex(retval, true);
  }
  return retval;
}

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
void FirmwareVersion() {
  if (!SILENCE) PGM_PRINTLN("\n=== 12.2.2 AltManufacturerAccess() 0x0002 Firmware Version");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::FIRMWARE_VERSION, buf, &len)) return;

  if (!SILENCE) {
    if (DEBUG) printBytesHex(buf, len);

    const word ddDD = composeWord(buf, 1, false);
    PGM_PRINT("Device Number: ");
    printWordHex(ddDD, true);

    const word vvVV = composeWord(buf, 3, false);
    PGM_PRINT("Version: ");
    printWordHex(vvVV, true);

    const word bbBB = composeWord(buf, 5, false);
    PGM_PRINT("Build number: ");
    printWordHex(bbBB, true);

    const word TT = buf[6];
    PGM_PRINT("Firmware type: ");
    printByteHex(TT, true);

    const word zzZZ = composeWord(buf, 8, false);
    PGM_PRINT("Impedance Track Version: ");
    printWordHex(zzZZ, true);
  }
}

/**
  @brief 12.2.3 AltManufacturerAccess() 0x0003 Hardware Version.

  The device can be checked for the hardware version of the IC.

  @returns the hardware revision on subsequent read on MACData()
*/
word HardwareVersion() {
  if (!SILENCE) PGM_PRINT("=== 12.2.3 AltManufacturerAccess() 0x0003 Hardware Version: ");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::HARDWARE_VERSION, buf, &len)) return 0;

  const word retval = composeWord(buf);
  if (!SILENCE) {
    printWordHex(retval, true);
    if (DEBUG) printBytesHex(buf, len);
  }
  return retval;
}

/**
  @brief 12.2.6 AltManufacturerAccess() 0x0006 Chemical ID.
  @returns the chemical ID of the OCV tables used in the gauging algorithm.
*/
word ChemicalID() {
  if (!SILENCE) PGM_PRINT("=== 12.2.6 AltManufacturerAccess() 0x0006 Chemical ID: ");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::CHEMICAL_ID, buf, &len)) return 0;

  const word retval = composeWord(buf);
  if (!SILENCE) {
    printWordHex(retval, true);
    if (DEBUG) printBytesHex(buf, len);
  }
  return retval;
}

/**
  @brief 12.2.12 AltManufacturerAccess() 0x0012 Device Reset

  This command resets the device.

  @warning [!] Not Available in SEALED Mode
*/
void DeviceReset() {
  if (!SILENCE) PGM_PRINTLN("=== 12.2.12 AltManufacturerAccess() 0x0012 Device Reset");
  AltManufacturerAccess(AltManufacturerCommands::DEVICE_RESET);
  delay(500);
}

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
void ChargeFET() {
  if (!SILENCE) PGM_PRINTLN("12.2.13 AltManufacturerAccess() 0x001F CHG FET");
  AltManufacturerAccess(AltManufacturerCommands::CHG_FET);
  delay(500);
}

/**
  @brief 12.2.14 AltManufacturerAccess() 0x0020 DSG FET

  This command turns on/off DSG FET drive function to ease testing during manufacturing.

  @warning [!] This toggling command will only set the RAM data,
               meaning the conditions set by the command will be cleared
               if a reset or seal is issued to the gauge.

  @warning [!] Not Available in SEALED Mode

  @see 12.2.16 AltManufacturerAccess() 0x0022 FETControl()
*/
void DischargeFET() {
  if (!SILENCE) PGM_PRINTLN("12.2.14 AltManufacturerAccess() 0x0020 DSG FET");
  AltManufacturerAccess(AltManufacturerCommands::DSG_FET);
  delay(500);
}

/**
  @brief 12.2.15 AltManufacturerAccess() 0x0021 Gauging

  This command enables or disables the gauging function to ease testing during manufacturing.

  @see AltManufacturerCommands
*/
void Gauging() {
  if (!SILENCE) PGM_PRINTLN("12.2.15 AltManufacturerAccess() 0x0021 Gauging");
  AltManufacturerAccess(AltManufacturerCommands::GAUGE_EN);
  delay(500);
}

/**
  @brief 12.2.16 AltManufacturerAccess() 0x0022 FET Control

  This command disables/enables control of the CHG, DSG, and PCHG FETs by the firmware.

  - Disable: ManufacturingStatus[FET_EN] = 1 -> 0, CHG and DSG FETs are disabled and remain OFF.
  - Enable:  ManufacturingStatus[FET_EN] = 0 -> 1, CHG and DSG FETs are controlled by the firmware

  @warning [!] Not Available in SEALED Mode
  @see ManufacturingStatus()
  @see ManufacturingStatusFlags
*/
void FETControl() {
  if (!SILENCE) PGM_PRINTLN("12.2.16 AltManufacturerAccess() 0x0022 FET Control");
  AltManufacturerAccess(AltManufacturerCommands::FET_CONTROL);
  delay(500);
}

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
void LifetimeDataReset() {
  if (!SILENCE) PGM_PRINTLN("12.2.19 AltManufacturerAccess() 0x0028 Lifetime Data Reset");
  AltManufacturerAccess(AltManufacturerCommands::LIFETIME_DATA_RESET);
}

/**
  @brief 12.2.20 AltManufacturerAccess() 0x0029 Permanent Fail Data Reset

  This command resets PF data in data flash for ease of manufacturing.

  @warning [!] Not Available in SEALED Mode.
*/
void PermanentFailureDataReset() {
  if (!SILENCE) PGM_PRINTLN("=== 12.2.20 AltManufacturerAccess() 0x0029 Permanent Fail Data Reset");
  AltManufacturerAccess(AltManufacturerCommands::PERMANENT_FAIL_DATA_RESET);
  delay(1000);  // needs some time to process
}

/**
  @brief 12.2.22 AltManufacturerAccess() 0x0030 Seal Device

  This command seals the device for the field, disabling certain commands and access to DF.

  @see unsealDevice()
  @see 9.5.2 SEALED to UNSEALED
*/
void SealDevice() {
  if (!SILENCE) PGM_PRINTLN("=== 12.2.22 AltManufacturerAccess() 0x0030 Seal Device");
  AltManufacturerAccess(AltManufacturerCommands::SEAL_DEVICE);
  delay(500);
}

/**
  @brief 12.2.26 AltManufacturerAccess() 0x0050 SafetyAlert
  @returns the SafetyAlert flags on AltManufacturerAccess() or MACData().
*/
u32 SafetyAlert() {
  if (!SILENCE) PGM_PRINTLN("\n=== 12.2.26 AltManufacturerAccess() 0x0050 SafetyAlert");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::SAFETY_ALERT, buf, &len)) return;

  const u32 retval = composeDoubleWord(buf);
  if (!SILENCE) printLongSplitBin(retval);

  return retval;
}

/**
  @brief 12.2.27 AltManufacturerAccess() 0x0051 SafetyStatus
  @returns the SafetyStatus flags on MACData().
  @see SafetyStatusFlags
*/
u32 SafetyStatus() {
  if (!SILENCE) PGM_PRINTLN("\n=== 12.2.27 AltManufacturerAccess() 0x0051 SafetyStatus");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::PF_ALERT, buf, &len)) return;

  const u32 retval = composeDoubleWord(buf);
  if (!SILENCE) printLongSplitBin(retval);

  return retval;
}

/**
  @brief 12.2.28 AltManufacturerAccess() 0x0052 PFAlert
  @returns the PFAlert flags on MACData().
*/
u32 PFAlert() {
  if (!SILENCE) PGM_PRINTLN("\n=== 12.2.28 AltManufacturerAccess() 0x0052 PFAlert");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::PF_ALERT, buf, &len)) return;

  const u32 retval = composeDoubleWord(buf);
  if (!SILENCE) printLongSplitBin(retval);

  return retval;
}

/**
  @brief 12.2.29 AltManufacturerAccess() 0x0053 PFStatus
  @returns the PFStatus flags on MACData().
  @see PFStatusFlags
*/
u32 PFStatus() {
  if (!SILENCE) PGM_PRINTLN("\n=== 12.2.28 AltManufacturerAccess() 0x0052 PFAlert");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::PF_STATUS, buf, &len)) return;

  const u32 retval = composeDoubleWord(buf);
  if (!SILENCE) printLongSplitBin(retval);

  return retval;
}

/**
  @brief 12.2.30 AltManufacturerAccess() 0x0054 OperationStatus
  @returns the OperationStatus flags on MACData()
  @see OperationStatusFlags
*/
u32 OperationStatus() {
  if (!SILENCE) PGM_PRINTLN("\n=== 12.2.30 AltManufacturerAccess() 0x0054 OperationStatus");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::OPERATION_STATUS, buf, &len)) return;

  const u32 operationStatus = composeDoubleWord(buf);
  if (!SILENCE) {
    printLongSplitBin(operationStatus);
    printFlag(operationStatus, OperationStatusFlags::EMSHUT());  // EMSHUT (Bit 29): Emergency FET Shutdown
    printFlag(operationStatus, OperationStatusFlags::CB());  // CB (Bit 28): Cell Balancing
    printFlag(operationStatus, OperationStatusFlags::SLPCC());  // SLPCC (Bit 27): CC Measurement in SLEEP mode
    printFlag(operationStatus, OperationStatusFlags::SLPAD());  // SLPAD (Bit 26): ADC Measurement in SLEEP mode
    printFlag(operationStatus, OperationStatusFlags::SMBLCAL());  // SMBLCAL (Bit 25): Auto-offset calibration when Bus low is detected
    printFlag(operationStatus, OperationStatusFlags::INIT());  // INIT (Bit 24): Initialization after full reset
    printFlag(operationStatus, OperationStatusFlags::SLEEPM());  // SLEEPM (Bit 23): SLEEP mode
    printFlag(operationStatus, OperationStatusFlags::XL());  // XL (Bit 22): 400-kHz mode
    printFlag(operationStatus, OperationStatusFlags::CAL_OFFSET());  // CAL_OFFSET (Bit 21): Calibration Output (raw CC Offset data)
    printFlag(operationStatus, OperationStatusFlags::CAL());  // CAL (Bit 20): Calibration Output (raw ADC and CC data)
    printFlag(operationStatus, OperationStatusFlags::AUTHCALM());  // AUTHCALM (Bit 19): Auto CC Offset Calibration by MAC AutoCCOffset()
    printFlag(operationStatus, OperationStatusFlags::AUTH());  // AUTH (Bit 18): Authentication in progress
    printFlag(operationStatus, OperationStatusFlags::SDM());  // SDM (Bit 16): SHUTDOWN triggered via command
    printFlag(operationStatus, OperationStatusFlags::SLEEP());  // SLEEP (Bit 15): SLEEP mode conditions met

    printFlag(operationStatus, OperationStatusFlags::XCHG());  // XCHG (Bit 14): Charging disabled
    printFlag(operationStatus, OperationStatusFlags::XDSG());  // XDSG (Bit 13): Discharging disabled
    printFlag(operationStatus, OperationStatusFlags::PF());  // PF (Bit 12): PERMANENT FAILURE mode status
    printFlag(operationStatus, OperationStatusFlags::SS());  // SS (Bit 11): SAFETY mode status
    printFlag(operationStatus, OperationStatusFlags::SDV());  // SDV (Bit 10): SHUTDOWN triggered via low pack voltage
    printFlag(operationStatus, OperationStatusFlags::SEC1());  // SEC1, SEC0 (Bits 9–8): SECURITY mode
    printFlag(operationStatus, OperationStatusFlags::SEC0());  // SEC1, SEC0 (Bits 9–8): SECURITY mode
    printFlag(operationStatus, OperationStatusFlags::BTP_INT());  // BTP_INT (Bit 7): Battery Trip Point (BTP) Interrupt output
    printFlag(operationStatus, OperationStatusFlags::CHG());  // CHG (Bit 2): CHG FET status
    printFlag(operationStatus, OperationStatusFlags::DSG());  // DSG (Bit 1): DSG FET status
  }
  return operationStatus;
}

/**
  @brief 12.2.31 AltManufacturerAccess() 0x0055 ChargingStatus
  @returns the ChargingStatus flags on MACData().
  @see ChargingStatusFlags
*/
word ChargingStatus() {
  if (!SILENCE) PGM_PRINTLN("\n=== 12.2.31 AltManufacturerAccess() 0x0055 ChargingStatus");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::CHARGING_STATUS, buf, &len)) return;

  const word chargingStatus = composeWord(buf);
  if (!SILENCE) {
    printWordBin(chargingStatus, true);
    printFlag(chargingStatus, ChargingStatusFlags::VCT());  // VCT (Bit 15): Charge Termination
    printFlag(chargingStatus, ChargingStatusFlags::MCHG());  // MCHG (Bit 14): Maintenance Charge
    printFlag(chargingStatus, ChargingStatusFlags::SU());  // SU (Bit 13): Charge Suspend
    printFlag(chargingStatus, ChargingStatusFlags::IN());  // IN (Bit 12): Charge Inhibit
    printFlag(chargingStatus, ChargingStatusFlags::HV());  // HV (Bit 11): High Voltage Region
    printFlag(chargingStatus, ChargingStatusFlags::MV());  // MV (Bit 10): Mid Voltage Region
    printFlag(chargingStatus, ChargingStatusFlags::LV());  // MV (Bit 10): Mid Voltage Region
    printFlag(chargingStatus, ChargingStatusFlags::PV());  // MV (Bit 10): Mid Voltage Region
    printFlag(chargingStatus, ChargingStatusFlags::OT());  // OT (Bit 6): Over Temperature Region
    printFlag(chargingStatus, ChargingStatusFlags::HT());  // HT (Bit 5): High Temperature Region
    printFlag(chargingStatus, ChargingStatusFlags::STH());  // STH (Bit 4): Standard Temperature High Region
    printFlag(chargingStatus, ChargingStatusFlags::RT());  // RT (Bit 3): Room Temperature Region
    printFlag(chargingStatus, ChargingStatusFlags::STL());  // RT (Bit 3): Room Temperature Region
    printFlag(chargingStatus, ChargingStatusFlags::LT());  // LT (Bit 1): Low Temperature Region
    printFlag(chargingStatus, ChargingStatusFlags::UT());  // UT (Bit 0): Under Temperature Region
  }
  return chargingStatus;
}

/**
  @brief 12.2.32 AltManufacturerAccess() 0x0056 GaugingStatus
  @returns the GaugingStatus flags on MACData().
  @see GaugingStatusFlags
*/
u32 GaugingStatus() {
  if (!SILENCE) PGM_PRINTLN("\n=== 12.2.32 AltManufacturerAccess() 0x0056 GaugingStatus");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::GAUGING_STATUS, buf, &len)) return;

  const u32 gaugingStatus = composeDoubleWord(buf);
  if (DEBUG) printLongSplitBin(gaugingStatus);
  if (!SILENCE) {
    printFlag(gaugingStatus, GaugingStatusFlags::OCVFR());  // OCVFR (Bit 20): Open Circuit Voltage in Flat Region (during RELAX)
    printFlag(gaugingStatus, GaugingStatusFlags::LDMD());  // LDMD (Bit 19): LOAD mode
    printFlag(gaugingStatus, GaugingStatusFlags::RX());  // RX (Bit 18): Resistance Update (Toggles after every resistance update)
    printFlag(gaugingStatus, GaugingStatusFlags::QMax());  // QMax (Bit 17): QMax Update (Toggles after every QMax update)
    printFlag(gaugingStatus, GaugingStatusFlags::VDQ());  // VDQ (Bit 16): Discharge Qualified for Learning (based on RU flag)
    printFlag(gaugingStatus, GaugingStatusFlags::NSFM());  // NSFM (Bit 15): Negative Scale Factor Mode
    printFlag(gaugingStatus, GaugingStatusFlags::SLPQMax());  // SLPQMax (Bit 13): QMax Update During Sleep
    printFlag(gaugingStatus, GaugingStatusFlags::QEN());  // QEN (Bit 12): Impedance Track Gauging (Ra and QMax updates are enabled.)
    printFlag(gaugingStatus, GaugingStatusFlags::VOK());  // VOK (Bit 11): Voltage OK for QMax Update
    printFlag(gaugingStatus, GaugingStatusFlags::RDIS());  // RDIS (Bit 10): Resistance Updates
    printFlag(gaugingStatus, GaugingStatusFlags::REST());  // REST (Bit 8): Rest
    printFlag(gaugingStatus, GaugingStatusFlags::CF());  // CF (Bit 7): Condition Flag
    printFlag(gaugingStatus, GaugingStatusFlags::DSG());  // DSG (Bit 6): Discharge/Relax
    printFlag(gaugingStatus, GaugingStatusFlags::EDV());  // EDV (Bit 5): End-of-Discharge Termination Voltage
    printFlag(gaugingStatus, GaugingStatusFlags::BAL_EN());  // BAL_EN (Bit 4): Cell Balancing
    printFlag(gaugingStatus, GaugingStatusFlags::TC());  // TC (Bit 3): Terminate Charge
    printFlag(gaugingStatus, GaugingStatusFlags::TD());  // TD (Bit 2): Terminate Discharge
    printFlag(gaugingStatus, GaugingStatusFlags::FC());  // FC (Bit 1): Fully Charged
    printFlag(gaugingStatus, GaugingStatusFlags::FD());  // FC (Bit 1): Fully Charged
  }
  return gaugingStatus;
}

/**
  @brief 12.2.33 AltManufacturerAccess() 0x0057 ManufacturingStatus
  @returns the ManufacturingStatus flags on MACData().
  @see ManufacturingStatusFlags
*/
word ManufacturingStatus() {
  if (!SILENCE) PGM_PRINTLN("\n=== 12.2.33 AltManufacturerAccess() 0x0057 ManufacturingStatus");
  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::MANUFACTURER_STATUS, buf, &len)) return;

  const word manufacturingStatus = composeWord(buf);
  if (DEBUG) printWordBin(manufacturingStatus, true);
  if (!SILENCE) {
    printFlag(manufacturingStatus, ManufacturingStatusFlags::CAL_EN());  // CAL_EN (Bit 15): CALIBRATION Mode
    printFlag(manufacturingStatus, ManufacturingStatusFlags::PF_EN());  // PF_EN (Bit 6): Permanent Failure
    printFlag(manufacturingStatus, ManufacturingStatusFlags::LF_EN());  // LF_EN (Bit 5): Lifetime Data Collection
    printFlag(manufacturingStatus, ManufacturingStatusFlags::FET_EN());  // FET_EN (Bit 4): All FET Action
    printFlag(manufacturingStatus, ManufacturingStatusFlags::GAUGE_EN());  // GAUGE_EN (Bit 3): Gas Gauging
    printFlag(manufacturingStatus, ManufacturingStatusFlags::DSG_TEST());  // DSG_TEST (Bit 2): Discharge FET Test
    printFlag(manufacturingStatus, ManufacturingStatusFlags::CHG_TEST());  // CHG_TEST (Bit 1): Charge FET Test
  }
  return manufacturingStatus;
}

/**
  @brief 12.2.37 AltManufacturerAccess() 0x0071 DAStatus1

  @returns 32 bytes of data on MACData() in the following format:
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
void DAStatus1(byte *retval) {
  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::DA_STATUS_1, buf, &len)) return;
  for (int i = 0; i < len; i++) retval[i] = buf[i];
}

/**
  @brief 12.2.37 AltManufacturerAccess() 0x0071 DAStatus1

  Print result of DAStatus1.
*/
void DAStatus1() {
  PGM_PRINTLN("\n=== 12.2.37 AltManufacturerAccess() 0x0071 DAStatus1");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE];
  DAStatus1(buf);

  const int cellVoltage1 = composeWord(buf, DA_STATUS_1::CELL_VOLTAGE_1);
  printPremil(PSTR("Cell Voltage 1"), cellVoltage1, Units::V());

  const int cellVoltage2 = composeWord(buf, DA_STATUS_1::CELL_VOLTAGE_2);
  printPremil(PSTR("Cell Voltage 2"), cellVoltage2, Units::V());

  const int batVoltage = composeWord(buf, DA_STATUS_1::BAT_VOLTAGE);
  printPremil(PSTR("BAT Voltage. Voltage at the VC2 (BAT) terminal"), batVoltage, Units::V());

  const int packVoltage = composeWord(buf, DA_STATUS_1::PACK_VOLTAGE);
  printPremil(PSTR("PACK Voltage"), packVoltage, Units::V());

  const int cellCurrent1 = composeWord(buf, DA_STATUS_1::CELL_CURRENT_1);
  printInteger(PSTR("Cell Current 1. Simultaneous current measured during Cell Voltage1 measurement"), cellCurrent1, Units::MA());

  const int cellCurrent2 = composeWord(buf, DA_STATUS_1::CELL_CURRENT_2);
  printInteger(PSTR("Cell Current 2. Simultaneous current measured during Cell Voltage2 measurement"), cellCurrent2, Units::MA());

  const int cellPower1 = composeWord(buf, DA_STATUS_1::CELL_POWER_1);
  printInteger(PSTR("Cell Power 1. Calculated using Cell Voltage1 and Cell Current 1 data"), cellPower1, Units::MW());

  const int cellPower2 = composeWord(buf, DA_STATUS_1::CELL_POWER_2);
  printInteger(PSTR("Cell Power 2. Calculated using Cell Voltage2 and Cell Current 2 data"), cellPower2, Units::MW());

  const int powerCalc = composeWord(buf, DA_STATUS_1::POWER);
  printInteger(PSTR("Power calculated by Voltage() × Current()"), powerCalc, Units::MW());

  const int avgPower = composeWord(buf, DA_STATUS_1::AVG_POWER);
  printInteger(PSTR("Average Power. Calculated by Voltage() × AverageCurrent()"), avgPower, Units::MW());
}

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
void ITStatus1(byte *retval) {
  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::IT_STATUS_1, buf, &len)) return;
  for (int i = 0; i < len; i++) retval[i] = buf[i];
}

/**
  @brief 12.2.39 AltManufacturerAccess() 0x0073 ITStatus1

  Print result of ITStatus1.
*/
void ITStatus1() {
  PGM_PRINTLN("\n=== 12.2.39 AltManufacturerAccess() 0x0073 ITStatus1");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  ITStatus1(buf);

  if (DEBUG) printBytesHex(buf, len);

  const int AAaa = composeValue(buf, 0, 1);
  printInteger(PSTR("AAaa: True Rem Q"), AAaa);

  const int BBbb = composeValue(buf, 2, 3);
  printInteger(PSTR("BBbb: True Rem E"), BBbb);

  const int CCcc = composeValue(buf, 4, 5);
  printInteger(PSTR("CCcc: Initial Q"), CCcc);

  const int DDdd = composeValue(buf, 6, 7);
  printInteger(PSTR("DDdd: Initial E"), DDdd);

  const int EEee = composeValue(buf, 8, 9);
  printInteger(PSTR("EEee: TrueFullChgQ"), EEee);

  const int FFff = composeValue(buf, 10, 11);
  printInteger(PSTR("FFff: TrueFullChgE"), FFff);

  const int GGgg = composeValue(buf, 12, 13);
  printInteger(PSTR("GGgg: T_sim"), GGgg);

  const int HHhh = composeValue(buf, 14, 15);
  printInteger(PSTR("HHhh: T_ambient"), HHhh);

  const int IIii = composeValue(buf, 16, 17);
  printInteger(PSTR("IIii: RaScale 0"), IIii);

  const int JJjj = composeValue(buf, 18, 19);
  printInteger(PSTR("JJjj: RaScale 1"), JJjj);

  const int KKkk = composeValue(buf, 20, 21);
  printInteger(PSTR("KKkk: CompRes1"), KKkk);

  const int LLll = composeValue(buf, 22, 23);
  printInteger(PSTR("LLll: CompRes2"), LLll);
}

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
void ITStatus2(byte *retval) {
  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::IT_STATUS_2, buf, &len)) return;
  for (int i = 0; i < len; i++) retval[i] = buf[i];
}

/**
  @brief 12.2.40 AltManufacturerAccess() 0x0074 ITStatus2

  Print result of ITStatus2.
*/
void ITStatus2() {
  PGM_PRINTLN("\n=== 12.2.40 AltManufacturerAccess() 0x0074 ITStatus2");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  ITStatus2(buf);

  if (DEBUG) printBytesHex(buf, len);

  const byte AA = buf[0];
  printInteger(PSTR("AA: Pack Grid. Active pack grid point (only valid in discharge)"), AA);

  const byte BB = buf[1];
  PGM_PRINT("BB: LStatus—Learned status of resistance table: ");
  printByteBin(BB, true);

  const byte QMaxStatus = BB & 0b11;
  PGM_PRINT("= QMax Status: ");
  if (0 == QMaxStatus) PGM_PRINTLN("Battery OK");
  else if (1 == QMaxStatus) PGM_PRINTLN("QMax is first updated in learning cycle.");
  else if (0b10 == QMaxStatus) PGM_PRINTLN("QMax and resistance table updated in learning cycle.");

  const bool ITEN = bitRead(BB, 2);
  printInteger(PSTR("= ITEN: IT enable"), ITEN);

  const bool QMax = bitRead(BB, 3);
  printInteger(PSTR("= QMax: QMax field updates"), QMax);

  const byte CC = buf[2];
  printInteger(PSTR("CC: Cell Grid 1. Active grid point of Cell1"), CC);

  const byte DD = buf[3];
  printInteger(PSTR("DD: Cell Grid 2. Active grid point of Cell2"), DD);

  const u32 HHhhGGgg = composeValue(buf, 6, 9);
  PGM_PRINT("HHhhGGgg: State Time. Time passed since last state change (Discharge, Charge, Rest): ");
  Serial.println(HHhhGGgg);

  const int IIii = composeValue(buf, 10, 11);
  printInteger(PSTR("IIii: DOD0_1. Depth of discharge for Cell1"), IIii);

  const int JJjj = composeValue(buf, 12, 13);
  printInteger(PSTR("JJjj: DOD0_2. Depth of discharge for Cell2"), JJjj);

  const int KKkk = composeValue(buf, 14, 15);
  printInteger(PSTR("KKkk: DOD0 Passed Q. Passed charge since DOD0"), KKkk);

  const int LLll = composeValue(buf, 16, 17);
  printInteger(PSTR("LLll: DOD0 Passed Energy. Passed energy since the last DOD0 update"), LLll);

  const int MMmm = composeValue(buf, 18, 19);
  printInteger(PSTR("MMmm: DOD0 Time. Time passed since the last DOD0 update"), MMmm);

  const int NNnn = composeValue(buf, 20, 21);
  printInteger(PSTR("NNnn: DODEOC_1. Cell 1 DOD@EOC"), NNnn);

  const int OOoo = composeValue(buf, 22, 23);
  printInteger(PSTR("OOoo: DODEOC_2. Cell 2 DOD@EOC"), OOoo);
}

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
void ITStatus3(byte *retval) {
  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  if (!AltManufacturerAccess(AltManufacturerCommands::IT_STATUS_3, buf, &len)) return;
  for (int i = 0; i < len; i++) retval[i] = buf[i];
}

/**
  @brief 12.2.41 AltManufacturerAccess() 0x0075 ITStatus3

  Print result of ITStatus3.
*/
void ITStatus3() {
  PGM_PRINTLN("\n=== 12.2.41 AltManufacturerAccess() 0x0075 ITStatus3");

  byte buf[BlockProtocol::RESPONSE_MAX_SIZE], len = 0;
  memset(buf, 0, sizeof(buf));
  ITStatus3(buf);

  if (DEBUG) printBytesHex(buf, len);

  const word QMax_1 = composeValue(buf, 0, 1);
  printInteger(PSTR("QMax 1. QMax of Cell 1"), QMax_1, Units::MAH());

  const word QMax_2 = composeValue(buf, 2, 3);
  printInteger(PSTR("QMax 2. QMax of Cell 2"), QMax_2, Units::MAH());

  const word QMaxDOD0_1 = composeValue(buf, 4, 5);
  printInteger(PSTR("QMaxDOD0_1. Cell 1 DOD for Qmax"), QMaxDOD0_1);

  const word QMaxDOD0_2 = composeValue(buf, 6, 7);
  printInteger(PSTR("QMaxDOD0_2. Cell 2 DOD for Qmax"), QMaxDOD0_2);

  const word QMaxPassedQ = composeValue(buf, 8, 9);
  printInteger(PSTR("QMaxPassedQ. Passed charge since DOD for Qmax recorded (mAh)"), QMaxPassedQ);

  const word QMaxTime = composeValue(buf, 10, 11);
  printInteger(PSTR("QMaxTime. Time since DOD for Qmax recorded (hour / 16 units)"), QMaxTime);

  const word Tk = composeValue(buf, 12, 13);
  printInteger(PSTR("Tk. Thermal model 'k'"), Tk);

  const word Ta = composeValue(buf, 14, 15);
  printInteger(PSTR("Ta. Thermal model 'a'"), Ta);

  const word RawDOD0_1 = composeValue(buf, 16, 17);
  printInteger(PSTR("RawDOD0_1. Cell 1 raw DOD0 measurement"), RawDOD0_1);

  const word RawDOD0_2 = composeValue(buf, 18, 19);
  printInteger(PSTR("RawDOD0_2. Cell 2 raw DOD0 measurement"), RawDOD0_2);
}
