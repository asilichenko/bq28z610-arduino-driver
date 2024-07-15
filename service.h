/**
  @file service.h

  @brief Custom functions headers

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
#include "std_data_commands.h"
#include "alt_manufacturer_access.h"
#include "data_flash_access.h"

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
int securityMode();

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
void unsealDevice(u32 key = DeviceSecurity::DEFAULT_UNSEAL_KEY);

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
void fullAccessDevice(u32 key = DeviceSecurity::DEFAULT_FULL_ACCESS_KEY);

/**
  @brief Brings Charge FET into desired mode.
  @see FETControl()
  @see ManufacturingStatusFlags
*/
void manufactoryChargeFet(bool mode);

/**
  @brief Brings Discharge FET into desired mode.
  @see FETControl()
  @see ManufacturingStatusFlags
  @see DischargeFET()
*/
void manufactoryDischargeFet(bool mode);

/**
  @brief Brings ManufacturingStatus[FET_EN] into desired mode.
  @see FETControl()
  @see ManufacturingStatusFlags
*/
void fetControl(bool mode);

/**
  @brief Chapter 3: Permanent Fail
  - OperationStatus()[PF] = 1
  - BatteryStatus()[TCA] = 1
  - BatteryStatus()[TDA] = 1

  @see OperationStatusFlags
  @see BatteryStatusFlags
*/
bool isPermanentFail();

/**
  @see DAStatus1()
  @see DA_STATUS_1
*/
float cellVoltage1();

/**
  @see DAStatus1()
  @see DA_STATUS_1
*/
float cellVoltage2();

/**
  @see DAStatus1()
  @see DA_STATUS_1
*/
float batVoltage();

/**
  @see DAStatus1()
  @see DA_STATUS_1
*/
float packVoltage();

/**
  @brief 2.2 Cell Undervoltage Protection
*/
void CellUndervoltageProtectionCheck();

/**
  @brief 2.6.2 Short Circuit in Charge Protection
*/
void ShortCircuitInChargeProtectionCheck();

/**
  @brief 2.6.3 Short Circuit in Discharge Protection
*/
void ShortCircuitInDischargeProtectionCheck();

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
void OvertemperatureInChargeProtectionCheck();

/**
  @brief Chapter 3 Permanent Fail
*/
void PermanentFailCheck();

/**
  @brief Print flags that correspond to the FETs status.
*/
void fetsStatus();

/**
  @brief Enable or Disable turning charging FET off at 60% SOC
  @see How to Stop Battery Charging at a Specific Percentage:
       https://www.linkedin.com/pulse/how-stop-battery-charging-specific-percentage-oleksii-sylichenko-vpmkf
  @see dfWriteFetOptionsCHGFET()
  @see dfWriteTcSetRsocThreshold()
  @see dfWriteTcClearRsocThreshold()
  @see dfWriteSocFlagConfigA()
*/
void setEnabledChargingSocThreshold(bool enabled);

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
void learningCycleInit(int designCapacityMah, int designEnergyCwh, word qMaxCell0, word qMaxCell1, word cycleCount);

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
void learningCycleLog();

/**
  @brief 12.2.40 AltManufacturerAccess() 0x0074 ITStatus2
  @returns KKkk: DOD0 Passed Q. Passed charge since DOD0
*/
int dod0PassedQ();

/**
  @brief Overcurrent in Charge trip threshold

  13.10.3 OCC - Overcurrent In Charge: Protections; OCC; 0x46C9; Threshold; I2
*/
int getOccThreshold();

/**
  @brief Overcurrent in Charge trip threshold

  13.10.3 OCC - Overcurrent In Charge: Protections; OCC; 0x46C9; Threshold; I2

  Should be not greater than C/2, where C - FullChargeCapacity.

  @see 12.1.10 0x12/13 FullChargeCapacity()
*/
void setOccThreshold(int occThreshold);
