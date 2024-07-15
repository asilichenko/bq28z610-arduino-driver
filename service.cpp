/**
  @file service.cpp

  @brief Custom functions implementation

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

#include "service.h"

/**
  @brief Request current security mode of the device.

  12.2.30 AltManufacturerAccess() 0x0054 OperationStatus()
  SEC1, SEC0 (Bits 14, 13): SECURITY Mode
    0, 0 = Reserved
    0, 1 = Full Access
    1, 0 = Unsealed
    1, 1 = Sealed

  12.1.1 0x00/01 ManufacturerAccessControl()
  SEC1, SEC0 (Bits 14, 13): SECURITY Mode
    0, 0 = Reserved
    0, 1 = Full Access - doesn't work correct! It's (0, 0) in fact
    1, 0 = Unsealed
    1, 1 = Sealed
*/
int securityMode() {
  const bool _silence = SILENCE;
  if (!DEBUG) SILENCE = true;

  const u32 operationStatus = OperationStatus();

  SILENCE = _silence;

  if (DEBUG) printLongSplitBin(operationStatus);

  const byte retval = (operationStatus >> OperationStatusFlags::SEC0().n) & 0b11;
  if (!SILENCE) {
    PGM_PRINT("=== Security mode: ");
    if (SecurityMode::SEALED == retval) PGM_PRINTLN("Sealed");
    else if (SecurityMode::UNSEALED == retval) PGM_PRINTLN("Unsealed");
    else if (SecurityMode::FULL_ACCESS == retval) PGM_PRINTLN("Full Access");
    else PGM_PRINTLN("Reserved");
  }
  return retval;
}

/**
  @brief 9.5.2 SEALED to UNSEALED

  SEALED to UNSEALED instructs the device to extend access to the standard and extended registers and
  data flash space and clears the [SEC1][SEC0] flags. In UNSEALED mode, all data, standard and extended
  registers and DF have read/write access. Unsealing is a two-step command performed by writing the first
  word of the unseal key to AltManufacturerAccess() (MAC), followed by the second word of the unseal key to
  AltManufacturerAccess(). The unseal key can be read and changed via the MAC SecurityKey() command when
  in FULL ACCESS mode. To return to the SEALED mode, either a hardware reset is needed,
  or the MAC SealDevice() command is needed to transit from FULL ACCESS or UNSEALED to SEALED.

  @see SealDevice()
  @see securityMode()
  @see DeviceSecurity::DEFAULT_UNSEAL_KEY
*/
void unsealDevice(u32 key) {
  if (!SILENCE) PGM_PRINTLN("=== unsealDevice");

  if (DEBUG) {
    PGM_PRINT("== Unseal key: ");
    printLongHex(key);
    Serial.println();
  }

  sendCommand(StdCommands::ALT_MANUFACTURER_ACCESS, key & 0xFFFF);
  delay(5);
  sendCommand(StdCommands::ALT_MANUFACTURER_ACCESS, (key >> 16) & 0xFFFF);

  delay(1000); // long delay is necessary
}

/**
  @brief 9.5.3 UNSEALED to FULL ACCESS

  UNSEALED to FULL ACCESS instructs the device to allow full access to all standard and extended registers
  and data flash. The device is shipped from TI in this mode. The keys for UNSEALED to FULL ACCESS can
  be read and changed via the MAC command SecurityKey() when in FULL ACCESS mode. Changing from
  UNSEALED to FULL ACCESS is performed by using the AltManufacturerAccess() command, by writing the first
  word of the Full Access Key to AltManufacturerAccess(), followed by the second word of the Full Access Key to
  AltManufacturerAccess(). In FULL ACCESS mode, the command to go to boot ROM can be sent.

  @see DeviceSecurity::DEFAULT_FULL_ACCESS_KEY
  @see SealDevice()
  @see securityMode()
*/
void fullAccessDevice(u32 key) {
  if (!SILENCE) PGM_PRINTLN("=== fullAccessDevice");
  unsealDevice(key);
}

/**
  @brief Brings Charge FET into desired mode.
  @see FETControl()
  @see ManufacturingStatusFlags
*/
void manufactoryChargeFet(bool mode) {
  const word manufacturingStatus = ManufacturingStatus();  // 12.2.33 AltManufacturerAccess() 0x0057 ManufacturingStatus
  const bool chg = bitRead(manufacturingStatus, ManufacturingStatusFlags::CHG_TEST().n);
  const bool fetEn = bitRead(manufacturingStatus, ManufacturingStatusFlags::FET_EN().n);

  // ManufacturingStatus()[FET_EN] should be '0'
  if (1 == fetEn) FETControl();  // 12.2.16 AltManufacturerAccess() 0x0022 FET Control

  if (chg == mode) return;  // only toggle in proper state

  const int _securityMode = securityMode();
  if (SecurityMode::SEALED == _securityMode) {
    // should remain unsealed
    unsealDevice();  // 9.5.2 SEALED to UNSEALED
  }

  // only toggle in proper state
  ChargeFET();  // 12.2.13 AltManufacturerAccess() 0x001F CHG FET
}

/**
  @brief Brings Discharge FET into desired mode.
  @see FETControl()
  @see ManufacturingStatusFlags
  @see DischargeFET()
*/
void manufactoryDischargeFet(bool mode) {
  const word manufacturingStatus = ManufacturingStatus();  // 12.2.33 AltManufacturerAccess() 0x0057 ManufacturingStatus
  const bool dsg = bitRead(manufacturingStatus, ManufacturingStatusFlags::DSG_TEST().n);
  const bool fetEn = bitRead(manufacturingStatus, ManufacturingStatusFlags::FET_EN().n);

  // ManufacturingStatus()[FET_EN] should be '0'
  if (1 == fetEn) FETControl();  // 12.2.16 AltManufacturerAccess() 0x0022 FET Control

  if (dsg == mode) return;  // only toggle in proper state

  const int _securityMode = securityMode();
  if (SecurityMode::SEALED == _securityMode) {
    // should remain unsealed
    unsealDevice();  // 9.5.2 SEALED to UNSEALED
  }

  DischargeFET();  // 12.2.14 AltManufacturerAccess() 0x0020 DSG FET
}

/**
  @brief Brings ManufacturingStatus[FET_EN] into desired mode.
  @see FETControl()
  @see ManufacturingStatusFlags
*/
void fetControl(bool mode) {
  const word manufacturingStatus = ManufacturingStatus();  // 12.2.33 AltManufacturerAccess() 0x0057 ManufacturingStatus
  const bool fetEn = bitRead(manufacturingStatus, ManufacturingStatusFlags::FET_EN().n);

  if (fetEn == mode) return;  // only toggle in proper state

  const int _securityMode = securityMode();
  if (SecurityMode::SEALED == _securityMode) unsealDevice();  // 9.5.2 SEALED to UNSEALED

  FETControl();  // 12.2.16 AltManufacturerAccess() 0x0022 FET Control

  if (SecurityMode::SEALED == _securityMode) SealDevice();  // 9.5.2 SEALED to UNSEALED
}

/**
  @brief Chapter 3: Permanent Fail
  - OperationStatus()[PF] = 1
  - BatteryStatus()[TCA] = 1
  - BatteryStatus()[TDA] = 1

  @see OperationStatusFlags
  @see BatteryStatusFlags
*/
bool isPermanentFail() {
  const bool _silence = SILENCE;
  SILENCE = true;
  const u32 operationStatus = OperationStatus();  // 12.2.30 AltManufacturerAccess() 0x0054 OperationStatus
  const word batteryStatus = BatteryStatus();
  const bool retval = bitRead(operationStatus, OperationStatusFlags::PF().n)
                      && bitRead(batteryStatus, BatteryStatusFlags::TCA().n)
                      && bitRead(batteryStatus, BatteryStatusFlags::TDA().n);
  SILENCE = _silence;
  if (!SILENCE) {
    if (retval) PGM_PRINTLN("\nThe device is in Permanent Fail!");
    else PGM_PRINTLN("\nPermanent Fail: Not detected");
  }
  return retval;
}

/**
  @see DAStatus1()
  @see DA_STATUS_1
*/
float cellVoltage1() {
  const bool _silence = SILENCE;
  SILENCE = true;

  byte daStatus1[BlockProtocol::MAX];
  DAStatus1(daStatus1);  // 12.2.37 AltManufacturerAccess() 0x0071 DAStatus1

  SILENCE = _silence;

  const int cellVoltage1 = composeWord(daStatus1, DA_STATUS_1::CELL_VOLTAGE_1);
  const float retval = PERMIL * cellVoltage1;

  if (!SILENCE) printFloat(PSTR("Cell Voltage 1"), retval, PERMIL_DECIMAL, Units::V());
  return retval;
}

/**
  @see DAStatus1()
  @see DA_STATUS_1
*/
float cellVoltage2() {
  const bool _silence = SILENCE;
  SILENCE = true;

  byte daStatus1[BlockProtocol::MAX];
  DAStatus1(daStatus1);  // 12.2.37 AltManufacturerAccess() 0x0071 DAStatus1

  SILENCE = _silence;

  const int cellVoltage2 = composeWord(daStatus1, DA_STATUS_1::CELL_VOLTAGE_2);
  const float retval = PERMIL * cellVoltage2;

  if (!SILENCE) printFloat(PSTR("Cell Voltage 2"), retval, PERMIL_DECIMAL, Units::V());
  return retval;
}

/**
  @see DAStatus1()
  @see DA_STATUS_1
*/
float batVoltage() {
  const bool _silence = SILENCE;
  SILENCE = true;

  byte daStatus1[BlockProtocol::MAX];
  DAStatus1(daStatus1);  // 12.2.37 AltManufacturerAccess() 0x0071 DAStatus1

  SILENCE = _silence;

  const int batVoltage = composeWord(daStatus1, DA_STATUS_1::BAT_VOLTAGE);
  const float retval = PERMIL * batVoltage;

  if (!SILENCE) printFloat(PSTR("BAT Voltage"), retval, PERMIL_DECIMAL, Units::V());
  return retval;
}

/**
  @see DAStatus1()
  @see DA_STATUS_1
*/
float packVoltage() {
  const bool _silence = SILENCE;
  SILENCE = true;

  byte daStatus1[BlockProtocol::MAX];
  DAStatus1(daStatus1);  // 12.2.37 AltManufacturerAccess() 0x0071 DAStatus1

  SILENCE = _silence;

  const int packVoltage = composeWord(daStatus1, DA_STATUS_1::PACK_VOLTAGE);
  const float retval = PERMIL * packVoltage;

  if (!SILENCE) printPremil(PSTR("PACK Voltage"), retval, Units::V());
  return retval;
}

/**
  @brief 2.2 Cell Undervoltage Protection
*/
void CellUndervoltageProtectionCheck() {
  PGM_PRINTLN("\n=== 2.2 Cell Undervoltage Protection:");

  const bool _silence = SILENCE;
  SILENCE = true;
  const u32 safetyAlert = SafetyAlert();  // 12.2.26 AltManufacturerAccess() 0x0050 SafetyAlert
  const u32 safetyStatus = SafetyStatus();  // 12.2.27 AltManufacturerAccess() 0x0051 SafetyStatus
  const word batteryStatus = BatteryStatus();  // 12.1.6 0x0A/0B BatteryStatus()
  const u32 operationStatus = OperationStatus();  // 12.2.30 AltManufacturerAccess() 0x0054 OperationStatus
  SILENCE = _silence;

  printFlag(PSTR("SafetyAlert()[CUV]"), safetyAlert, SafetyAlertFlags::CUV);
  printFlag(PSTR("SafetyStatus()[CUV]"), safetyStatus, SafetyStatusFlags::CUV);
  printFlag(PSTR("BatteryStatus()[TDA]"), batteryStatus, BatteryStatusFlags::TDA);
  printFlag(PSTR("BatteryStatus()[FD]"), batteryStatus, BatteryStatusFlags::FD);
  printFlag(PSTR("BatteryStatus()[TDA]"), batteryStatus, BatteryStatusFlags::TDA);
  printFlag(PSTR("OperationStatus()[XDSG]"), operationStatus, OperationStatusFlags::XDSG);
}

/**
  @brief 2.6.2 Short Circuit in Charge Protection
*/
void ShortCircuitInChargeProtectionCheck() {
  PGM_PRINTLN("\n=== 2.6.2 Short Circuit in Charge Protection:");

  const bool _silence = SILENCE;
  SILENCE = true;
  const u32 safetyAlert = SafetyAlert();  // 12.2.26 AltManufacturerAccess() 0x0050 SafetyAlert
  const u32 safetyStatus = SafetyStatus();  // 12.2.27 AltManufacturerAccess() 0x0051 SafetyStatus
  const word batteryStatus = BatteryStatus();  // 12.1.6 0x0A/0B BatteryStatus()
  const u32 operationStatus = OperationStatus();  // 12.2.30 AltManufacturerAccess() 0x0054 OperationStatus
  SILENCE = _silence;

  printFlag(PSTR("SafetyAlert()[ASCC]"), safetyAlert, SafetyAlertFlags::ASCC);
  printFlag(PSTR("SafetyStatus()[ASCC]"), safetyStatus, SafetyStatusFlags::ASCC);
  printFlag(PSTR("BatteryStatus()[TCA]"), batteryStatus, BatteryStatusFlags::TCA);
  printFlag(PSTR("OperationStatus()[XCHG]"), operationStatus, OperationStatusFlags::XCHG);
}

/**
  @brief 2.6.3 Short Circuit in Discharge Protection
*/
void ShortCircuitInDischargeProtectionCheck() {
  PGM_PRINTLN("\n=== 2.6.3 Short Circuit in Discharge Protection:");

  const bool _silence = SILENCE;
  SILENCE = true;
  const u32 safetyAlert = SafetyAlert();  // 12.2.26 AltManufacturerAccess() 0x0050 SafetyAlert
  const u32 safetyStatus = SafetyStatus();  // 12.2.27 AltManufacturerAccess() 0x0051 SafetyStatus
  SILENCE = _silence;

  printFlag(PSTR("SafetyAlert()[ASCD]"), safetyAlert, SafetyAlertFlags::ASCD);
  printFlag(PSTR("SafetyStatus()[ASCD]"), safetyStatus, SafetyStatusFlags::ASCD);
  printFlag(PSTR("OperationStatus()[XDSG]"), safetyStatus, OperationStatusFlags::XDSG);
}

/**
  @brief 2.8 Overtemperature in Charge Protection

  Trip:
  - SafetyAlert()[OTC] = 0
  - SafetyStatus()[OTC] = 1
  - BatteryStatus()[OTA] = 1
  - BatteryStatus()[TCA] = 0
  - OperationStatus()[XCHG] = 1

  @see SafetyAlertFlags
  @see SafetyStatusFlags
  @see BatteryStatusFlags
  @see OperationStatusFlags
  @see DF_ADDR::OTC_THRESHOLD
  @see DF_ADDR::OTC_RECOVERY
*/
void OvertemperatureInChargeProtectionCheck() {
  PGM_PRINTLN("\n=== 2.8 Overtemperature in Charge Protection");
  const boolean silence = SILENCE;

  SILENCE = true;
  const u32 safetyAlert = SafetyAlert();  // 12.2.26 AltManufacturerAccess() 0x0050 SafetyAlert
  const u32 safetyStatus = SafetyStatus();  // 12.2.27 AltManufacturerAccess() 0x0051 SafetyStatus
  const word batteryStatus = BatteryStatus();  // 12.1.6 0x0A/0B BatteryStatus()
  const u32 operationStatus = OperationStatus();  // 12.2.30 AltManufacturerAccess() 0x0054 OperationStatus

  // 13.10.8 OTC—Overtemperature in Charge. Protections.OTC
  const float otcThreshold = DECIPART * dfReadI2(DF_ADDR::OTC_THRESHOLD);  // 46.0 °C
  const float otcRecovery = DECIPART * dfReadI2(DF_ADDR::OTC_RECOVERY);  // 43.0 °C

  SILENCE = false;
  Temperature();  // 12.1.4 0x06/07 Temperature()
  ChargingVoltage();  // 12.1.25 0x30/31 ChargingVoltage()
  ChargingCurrent();  // 12.1.26 0x32/33 ChargingCurrent()
  printFlag(PSTR("SafetyAlert()[OTC]"), safetyAlert, SafetyAlertFlags::OTC);  // OTC (Bit 12): Overtemperature During Charge
  printFlag(PSTR("SafetyStatus()[OTC]"), safetyStatus, SafetyStatusFlags::OTC);  // OTC (Bit 12): Overtemperature During Charge
  printFlag(PSTR("BatteryStatus()[OTA]"), batteryStatus, BatteryStatusFlags::OTA);  // OTA (Bit 12): Overtemperature Alarm
  printFlag(PSTR("BatteryStatus()[TCA]"), batteryStatus, BatteryStatusFlags::TCA);  // TCA (Bit 14): Terminate Charge Alarm
  printFlag(PSTR("OperationStatus()[XCHG]"), operationStatus, OperationStatusFlags::XCHG);  // XCHG (Bit 14): Charging disabled
  Serial.println();
  printFloat(PSTR("OTCThreshold"), otcThreshold, DECIPART_DECIMAL, Units::CELSIUS);
  printFloat(PSTR("OTCRecovery"), otcRecovery, DECIPART_DECIMAL, Units::CELSIUS);
  SILENCE = silence;
}

/**
  @brief Chapter 3 Permanent Fail

  @see OperationStatus()
  @see BatteryStatus()
  @see ChargingCurrent()
  @see ChargingVoltage()
  @see OperationStatusFlags
  @see BatteryStatusFlags
*/
void PermanentFailCheck() {
  PGM_PRINTLN("\n=== 3 Permanent Fail");

  const bool _silence = SILENCE;
  SILENCE = true;
  const word batteryStatus = BatteryStatus();  // 12.1.6 0x0A/0B BatteryStatus()
  const u32 operationStatus = OperationStatus();  // 12.2.30 AltManufacturerAccess() 0x0054 OperationStatus
  const float chargingVoltage = ChargingVoltage();  // 12.1.25 0x30/31 ChargingVoltage()
  const int chargingCurrent = ChargingCurrent();  // 12.1.26 0x32/33 ChargingCurrent()
  SILENCE = _silence;

  printFlag(PSTR("OperationStatus()[PF]"), operationStatus, OperationStatusFlags::PF);
  printFlag(PSTR("BatteryStatus()[TCA]"), batteryStatus, BatteryStatusFlags::TCA);
  printFlag(PSTR("BatteryStatus()[TDA]"), batteryStatus, BatteryStatusFlags::TDA);
  printInteger(PSTR("ChargingCurrent()"), chargingCurrent, Units::MA());
  printFloat(PSTR("ChargingVoltage()"), chargingVoltage, PERMIL_DECIMAL, Units::V());
}

/**
  @brief Print flags that correspond to the FETs status.

  @see ChargingCurrent()
  @see ChargingVoltage()
  @see ManufacturingStatus()
  @see ManufacturingStatusFlags
  @see OperationStatus()
  @see OperationStatusFlags
*/
void fetsStatus() {
  PGM_PRINTLN("\n=== FETs Status:");

  const bool _silence = SILENCE;
  SILENCE = false;

  securityMode();

  const int chargingCurrent = ChargingCurrent();  // 12.1.26 0x32/33 ChargingCurrent()
  const float chargingVoltage = ChargingVoltage();  // 12.1.25 0x30/31 ChargingVoltage()

  SILENCE = true;

  const word manufacturingStatus = ManufacturingStatus();  // 12.2.33 AltManufacturerAccess() 0x0057 ManufacturingStatus
  printFlag(PSTR("ManufacturingStatus()[FET_EN]"), manufacturingStatus, ManufacturingStatusFlags::FET_EN);
  printFlag(PSTR("ManufacturingStatus()[CHG]"), manufacturingStatus, ManufacturingStatusFlags::CHG_TEST);
  printFlag(PSTR("ManufacturingStatus()[DSG]"), manufacturingStatus, ManufacturingStatusFlags::DSG_TEST);

  const u32 operationStatus = OperationStatus();  // 12.2.30 AltManufacturerAccess() 0x0054 OperationStatus
  printFlag(PSTR("OperationStatus()[XCHG]"), operationStatus, OperationStatusFlags::XCHG);
  printFlag(PSTR("OperationStatus()[XDSG]"), operationStatus, OperationStatusFlags::XDSG);

  SILENCE = _silence;
}

/**
  @brief Enable or Disable turning charging FET off at 60% SOC
  @see How to Stop Battery Charging at a Specific Percentage:
       https://www.linkedin.com/pulse/how-stop-battery-charging-specific-percentage-oleksii-sylichenko-vpmkf
  @see dfWriteFetOptionsCHGFET()
  @see dfWriteTcSetRsocThreshold()
  @see dfWriteTcClearRsocThreshold()
  @see dfWriteSocFlagConfigA()
*/
void setEnabledChargingSocThreshold(bool enabled) {
  dfWriteFetOptionsCHGFET(enabled);  // If FET Options[CHGFET] = 1 and GaugingStatus()[TC] = 1, CHG FET turns off

  if (!enabled) return;

  const byte STOP_THRESHOLD = 60, RESUME_THRESHOLD = 55;

  const byte setThreshold = dfReadTcSetRsocThreshold();
  if (STOP_THRESHOLD != setThreshold) dfWriteTcSetRsocThreshold(STOP_THRESHOLD);

  const byte clearThreshold = dfReadTcClearRsocThreshold();
  if (RESUME_THRESHOLD != clearThreshold) dfWriteTcClearRsocThreshold(RESUME_THRESHOLD);

  word socFlagConfigA = dfReadSocFlagConfigA();
  const word temp = socFlagConfigA;

  bitClear(socFlagConfigA, SOCFlagConfigAFlags::TCSETV().n);  // Disable the TC flag set by cell voltage threshold
  bitClear(socFlagConfigA, SOCFlagConfigAFlags::TCCLEARV().n);  // Disable the TC flag clear by cell voltage threshold

  bitSet(socFlagConfigA, SOCFlagConfigAFlags::TCSETRSOC().n);  // Enables the TC flag set by the RSOC threshold
  bitSet(socFlagConfigA, SOCFlagConfigAFlags::TCCLEARRSOC().n);  // Enables the TC flag cleared by the RSOC threshold

  if (temp != socFlagConfigA) dfWriteSocFlagConfigA(socFlagConfigA);
}

/**
  @brief 12.2.40 AltManufacturerAccess() 0x0074 ITStatus2
  @returns KKkk: DOD0 Passed Q. Passed charge since DOD0
*/
int dod0PassedQ() {
  byte buf[BlockProtocol::RESPONSE_MAX_SIZE];
  memset(buf, 0, sizeof(buf));
  ITStatus2(buf);
  return composeWord(buf, IT_STATUS_2::DOD0_PASSED_Q);  // KKkk: DOD0 Passed Q. Passed charge since DOD0
}

/**
  @brief Write initial parameters to the Gas Gauging Device.

  Write into Data Flash:
  - Design Capacity mAh (pack Capacity) to address 0x462A;
  - Design Energy cWh (pack Voltage * pack Capacity / 10) to 0x462C;
  - expected Qmax Cell 0 word to 0x4206;
  - expected Qmax Cell 1 word to 0x4208;
  - expected Qmax Pack word to 0x420A;
  - Gas Gauging Update Status: byte = 0x04 to 0x420E.
  - Gas Gauging: Cycle Count word = 0 - for new battery, or some approx. value of recharging cycles - for used one to 0x4240;

  Reset "R_a flags" in the Data Flash to the Default values:
  - Cell0 R_a flag: addr = 0x4100, data = 0xFF55 - Cell impedance never updated; Table being used;
  - Cell1 R_a flag: addr = 0x4140, data = 0xFF55 - Cell impedance never updated; Table being used;
  - xCell0 R_a flag: addr = 0x4180, data = 0xFFFF - Cell impedance never updated; Table never used;
  - xCell0 R_a flag: addr = 0x41C0, data = 0xFFFF - Cell impedance never updated; Table never used;

  @see https://www.linkedin.com/pulse/gas-gauging-device-bq28z610-learning-cycle-practical-guide-oleksii-ngk8f
*/
void learningCycleInit(int designCapacityMah, int designEnergyCwh, word qMaxCell0, word qMaxCell1, word cycleCount) {
  static const byte UPDATE_STATUS_LEARNING = 0x04;

  dfWriteDesignCapacityMah(designCapacityMah);
  dfWriteDesignCapacityCwh(designEnergyCwh);
  dfWriteQmax(qMaxCell0, qMaxCell1);
  dfWriteGasGaugingUpdateStatus(UPDATE_STATUS_LEARNING);
  dfWriteCycleCount(cycleCount);

  // Reset "R_a flags" in the Data Flash to the Default values
  dfResetRaTableFlags();
}

/**
  @brief Print into Serial port the important information for the Learning Cycle.
  - Cell0 Voltage, mV
  - Cell1 Voltage, mV
  - Pack Voltage, mV
  - Momentary current, mA
  - Temperature, 0.1*C
  - State Of Charge (SOC), %
  - QMax of the Cell0, mAh
  - QMax of the Cell1, mAh
  - QMax of the Pack, mAh
  - Binary result of GaugingStatus()
  - Update Status in HEX format

  @see https://www.linkedin.com/pulse/gas-gauging-device-bq28z610-learning-cycle-practical-guide-oleksii-ngk8f
*/
void learningCycleLog() {
  const bool _silence = SILENCE;
  SILENCE = true;

  byte buf[BlockProtocol::MAX];
  memset(buf, 0, sizeof(buf));
  DAStatus1(buf);  // 12.2.37 AltManufacturerAccess() 0x0071 DAStatus1

  const word cellVoltage1 = composeWord(buf, DA_STATUS_1::CELL_VOLTAGE_1);
  const word cellVoltage2 = composeWord(buf, DA_STATUS_1::CELL_VOLTAGE_2);
  const word packVoltage = composeWord(buf, DA_STATUS_1::PACK_VOLTAGE);

  const word soc = RelativeStateOfCharge();  // 12.1.23 0x2C/2D RelativeStateOfCharge()
  const u32 gaugingStatus = GaugingStatus();  // 12.2.32 AltManufacturerAccess() 0x0056 GaugingStatus

  const int current = Current();
  const float t = Temperature();

  const int qMaxCell1 = dfReadQmaxCell1();
  const int qMaxCell2 = dfReadQmaxCell2();
  const int qMaxPack = dfReadQmaxPack();
  const byte updateStatus = dfReadGasGaugingUpdateStatus();

  Serial.println("cellVoltage1:" + String(cellVoltage1)
                 + ",cellVoltage2:" + String(cellVoltage2)
                 + ",pack:" + String(packVoltage)
                 + ",current:" + String(current)
                 + ",t:" + String(t, 1)
                 + ",soc:" + String(soc)
                 + ",qMaxCell1:" + String(qMaxCell1)
                 + ",qMaxCell2:" + String(qMaxCell2)
                 + ",qMaxPack:" + String(qMaxPack)
                 + ",gaugingStatus:" + String(gaugingStatus, BIN)
                 + ",updateStatus:" + String(updateStatus, HEX)
                );
  SILENCE = _silence;
}

/**
  @brief Overcurrent in Charge trip threshold

  13.10.3 OCC - Overcurrent In Charge: Protections; OCC; 0x46C9; Threshold; I2
*/
int getOccThreshold() {
  return dfReadI2(DF_ADDR::OCC_THRESHOLD);
}

/**
  @brief Overcurrent in Charge trip threshold

  13.10.3 OCC - Overcurrent In Charge: Protections; OCC; 0x46C9; Threshold; I2

  Should be not greater than C/2, where C - FullChargeCapacity.

  @see 12.1.10 0x12/13 FullChargeCapacity()
*/
void setOccThreshold(int occThreshold) {
  dfWriteI2(DF_ADDR::OCC_THRESHOLD, occThreshold);
}
