/**
  @file std_data_commands.h
  
  @brief 12.1 Standard Data Commands headers

  The host controller communicates with the gauge through gauge commands.
  A gauge command is the equivalent to a register. For example, reading the state of charge is
  accomplished through the StateOfCharge() gauge command, which has the command codes 0x1C and
  0x1D. So if the interface is I2C, reading from device register 0x1C and 0x1D returns the current state of
  charge of the battery as calculated by the gauge.

  To enable the system to read and write battery information, the BQ28Z610 uses a series of 2-byte standard
  I2C commands with a 7-bit device address of 0x55 (8 bits = 0xAA to write and 0xAB to read). Each standard
  command has an associated command code pair, as indicated in Table 12-1, Standard Commands. Each
  protocol has specific means to access the data at each command code. Data RAM is updated and read by the
  gauge once per second.

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

#include <Arduino.h>

#include "globals.h"
#include "utils.h"

/**
  @brief 12.1.1 0x00/01 ManufacturerAccessControl

  A read on this register returns the Control bits.

  This Control Register is an I2C register, and the control bits are read back on register 0x00/0x01.
  These control bits are provided for backward compatibility/ease-of-use.

  - SEC1 (Bit 14): SECURITY Mode
  - SEC0 (Bit 13): SECURITY Mode
  - AUTHCALM (Bit 12): Automatic CALIBRATION mode
  - CheckSumValid (Bit 9): Checksum Valid
  - BTP_INT (Bit 7): Battery Trip Point Interrupt
  - LDMD (Bit 3): LOAD Mode
  - R_DIS (Bit 2): Resistance Updates
  - VOK (Bit 1): Voltage OK for QMax Update
  - QMax (Bit 0): QMax Updates. This bit toggles after every QMax update

  @warning [!] SEC1, SEC0 (Bits 14, 13): SECURITY Mode - doesn't work correct!\n
               It's (0, 0) for Full Access but should be (1, 0).\n
               Use function "12.2.30 AltManufacturerAccess() 0x0054 OperationStatus()" instead.
*/
word ManufacturerAccessControl();

/**
  @brief 12.1.4 0x06/07 Temperature
  @returns an unsigned integer value of temperature in units ( 0.1 k)
  measured by the gas gauge, and is used for the gauging algorithm.
  It reports either InternalTemperature() or external thermistor temperature,
  depending on the setting of the [TEMPS] bit in Pack configuration.
*/
float Temperature();

/**
  @brief 12.1.5 0x08/09 Voltage
  @returns the sum of the measured cell voltages.
*/
float Voltage();

/**
  @brief 12.1.6 0x0A/0B BatteryStatus

  <pre>
  Bits 3:0: EC3,EC2,EC1,EC0 - Error Code:
    0x00 = OK
    0x1 = Busy
    0x2 = Reserved Command
    0x3 = Unsupported Command
    0x4 = AccessDenied
    0x5 = Overflow/Underflow
    0x6 = BadSize
    0x7 = UnknownError
  Bit 4: FD—Fully Discharged
    0 = Battery ok
    1 = Battery fully depleted
  Bit 5: FC—Fully Charged
    0 = Battery not fully charged
    1 = Battery fully charged
  Bit 6: DSG—Discharging
    0 = Battery is charging.
    1 = Battery is discharging.
  Bit 7: INIT—Initialization
    0 = Inactive
    1 = Active
  Bit 8: RTA—Remaining Time Alarm
    0 = Inactive
    1 = Active
  Bit 9: RCA—Remaining Capacity Alarm
    0 = Inactive
    1 = Active
  Bit 10: Reserved
  Bit 11: TDA—Terminate Discharge Alarm
    0 = Inactive
    1 = Active
  Bit 12: OTA—Overtemperature Alarm
    0 = Inactive
    1 = Active
  Bit 13: Reserved
  Bit 14: TCA—Terminate Charge Alarm
    0 = Inactive
    1 = Active
  Bit 15: OCA—Overcharged Alarm
    0 = Inactive
    1 = Active
  </pre>
*/
word BatteryStatus();

/**
  @brief 12.1.7 0x0C/0D Current
  @returns the measured current from the coulomb counter.
*/
int Current();

/**
  @brief 12.1.9 0x10/11 RemainingCapacity
  @returns the predicted remaining battery capacity.
*/
word RemainingCapacity();

/**
  @brief 12.1.10 0x12/13 FullChargeCapacity
  @returns the predicted battery capacity when fully charged.
*/
word FullChargeCapacity();

/**
  @brief 12.1.11 0x14/15 AverageCurrent

  The value is updated every 1s. Units are mA.

  @returns a signed integer value that is the average current flow through the sense resistor.
*/
int AverageCurrent();

/**
  @brief 12.1.22 0x2A/2B CycleCount
  @returns the number of discharge cycles the battery has experienced.
*/
word CycleCount();

/**
  @brief 12.1.23 0x2C/2D RelativeStateOfCharge
  @returns the predicted remaining battery capacity as a percentage of FullChargeCapacity().
*/
word RelativeStateOfCharge();

/**
  @brief 12.1.24 0x2E/2F State-of-Health (SOH)
  @returns the state-of-health (SOH) information of the battery in percentage of design capacity.
*/
word StateOfHealth();

/**
  @brief 12.1.25 0x30/31 ChargingVoltage
  @returns the desired charging voltage.
*/
float ChargingVoltage();

/**
  @brief 12.1.26 0x32/33 ChargingCurrent
  @returns the desired charging current.
*/
word ChargingCurrent();

/**
  @brief 12.1.27 0x3C/3D DesignCapacity
  @returns the theoretical maximum pack capacity.
*/
word DesignCapacity();
