/**
  @file bq28z610-arduino-driver.ino

  This is BQ28Z610 Battery gas gauge device driver for Arduino, based on the documentation:

  Technical Reference Manual
  (c) Texas Instruments
  Literature Number: SLUUA65E
  https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf

  BQ28Z610 product: https://www.ti.com/product/BQ28Z610

  Arduino Nano standard I2C connection:
  - GND -> black
  - A4 = SDA -> white
  - A5 = SCL -> yellow


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

#include <Wire.h>

#include "globals.h"
#include "flags.h"
#include "utils.h"

#include "std_data_commands.h"
#include "alt_manufacturer_access.h"
#include "data_flash_access.h"

#include "service.h"

bool SILENCE = false,  // true = do not print results inside functions
     DEBUG = false;    // true = print extra raw data

void setup() {
  delay(5000);  // Prevent running when resetting while uploading sketch to Arduino

  Wire.begin();  // initializes the Wire library and join the I2C bus as a controller device

  Serial.begin(9600);  // start serial for output
  while (!Serial) {};  // wait until serial port is ready
  Serial.println(stringFromProgmem(START_MESSAGE));

  //
  // Sample code
  //

  if (SecurityMode::SEALED == securityMode()) unsealDevice();

  isPermanentFail();  // ..... Check whether the Device in Permanent Fail
  Voltage();  // ............. 12.1.5 0x08/09 Voltage()
  Current();  // ............. 12.1.7 0x0C/0D Current()
  Temperature();  // ......... 12.1.4 0x06/07 Temperature()
  ChargingVoltage();  // ..... 12.1.25 0x30/31 ChargingVoltage()
  ChargingCurrent(); // ...... 12.1.26 0x32/33 ChargingCurrent()
  DesignCapacity();  // ...... 12.1.27 0x3C/3D DesignCapacity()
  FullChargeCapacity();  // .. 12.1.10 0x12/13 FullChargeCapacity()
  RemainingCapacity();  // ... 12.1.9 0x10/11 RemainingCapacity()
  RelativeStateOfCharge();  // 12.1.23 0x2C/2D RelativeStateOfCharge()
  BatteryStatus();  // ....... 12.1.6 0x0A/0B BatteryStatus()
  DAStatus1();  // ........... 12.2.37 AltManufacturerAccess() 0x0071 DAStatus1
  ITStatus1();  // ........... 12.2.39 AltManufacturerAccess() 0x0073 ITStatus1
  ITStatus2();  // ........... 12.2.40 AltManufacturerAccess() 0x0074 ITStatus2
  ITStatus3();  // ........... 12.2.41 AltManufacturerAccess() 0x0075 ITStatus3
  GaugingStatus();  // ....... 12.2.32 AltManufacturerAccess() 0x0056 GaugingStatus

  //
  // Chapters 13 and 14 Data Flash Access

  dfDeviceName();  // .......... Device Name from the Data Flash
  dfReadDesignCapacityMah();  // Design Capacity in mAh from the Data Flash
  dfReadDesignCapacityCwh();  // Design Capacity in cWh from the Data Flash
  Serial.println();

  //
  // 12.1 Standard Data Commands

  Temperature();  // ......... 12.1.4 0x06/07 Temperature()
  Voltage();  // ............. 12.1.5 0x08/09 Voltage()
  BatteryStatus();  // ....... 12.1.6 0x0A/0B BatteryStatus()
  Current();  // ............. 12.1.7 0x0C/0D Current()
  RemainingCapacity();  // ... 12.1.9 0x10/11 RemainingCapacity()
  FullChargeCapacity();  // .. 12.1.10 0x12/13 FullChargeCapacity()
  AverageCurrent();  // ...... 12.1.11 0x14/15 AverageCurrent()
  CycleCount();  // .......... 12.1.22 0x2A/2B CycleCount()
  RelativeStateOfCharge();  // 12.1.23 0x2C/2D RelativeStateOfCharge()
  StateOfHealth();  // ....... 12.1.24 0x2E/2F State-of-Health (SOH)
  ChargingVoltage();  // ..... 12.1.25 0x30/31 ChargingVoltage()
  ChargingCurrent(); // ...... 12.1.26 0x32/33 ChargingCurrent()
  DesignCapacity();  // ...... 12.1.27 0x3C/3D DesignCapacity()
  Serial.println();

  //
  // 12.2 0x00, 0x01 AltManufacturerAccess() and 0x3E, 0x3F AltManufacturerAccess()

  DeviceType();  // ........ 12.2.1 AltManufacturerAccess() 0x0001 Device Type.
  FirmwareVersion();  // ... 12.2.2 AltManufacturerAccess() 0x0002 Firmware Version.
  HardwareVersion();  // ... 12.2.3 AltManufacturerAccess() 0x0003 Hardware Version.
  ChemicalID();  // ........ 12.2.6 AltManufacturerAccess() 0x0006 Chemical ID.
  SafetyAlert();  // ....... 12.2.26 AltManufacturerAccess() 0x0050 SafetyAlert
  SafetyStatus();  // ...... 12.2.27 AltManufacturerAccess() 0x0051 SafetyStatus
  PFAlert();  // ........... 12.2.28 AltManufacturerAccess() 0x0052 PFAlert
  PFStatus();  // .......... 12.2.29 AltManufacturerAccess() 0x0053 PFStatus
  OperationStatus();  // ... 12.2.30 AltManufacturerAccess() 0x0054 OperationStatus
  ChargingStatus();  // .... 12.2.31 AltManufacturerAccess() 0x0055 ChargingStatus
  GaugingStatus();  // ..... 12.2.32 AltManufacturerAccess() 0x0056 GaugingStatus
  ManufacturingStatus();  // 12.2.33 AltManufacturerAccess() 0x0057 ManufacturingStatus
  DAStatus1();  // ......... 12.2.37 AltManufacturerAccess() 0x0071 DAStatus1
  ITStatus1();  // ......... 12.2.39 AltManufacturerAccess() 0x0073 ITStatus1
  ITStatus2();  // ......... 12.2.40 AltManufacturerAccess() 0x0074 ITStatus2
  ITStatus3();  // ......... 12.2.41 AltManufacturerAccess() 0x0075 ITStatus3
  Serial.println();

  //
  // Chapter 2 Protections. Print flags

  CellUndervoltageProtectionCheck();  // print flags for: "2.2 Cell Undervoltage Protection"
  ShortCircuitInChargeProtectionCheck();  // print flags for: "2.6.2 Short Circuit in Charge Protection"
  ShortCircuitInDischargeProtectionCheck();  // print flags for: "2.6.3 Short Circuit in Discharge Protection"
  OvertemperatureInChargeProtectionCheck();  // print flags for: 2.8 Overtemperature in Charge Protection
  //
  PermanentFailCheck();  // print flags for: "Chapter 3 Permanent Fail"
  isPermanentFail();  // check whether the Device in Permanent Fail

  SILENCE = true;
  DEBUG = false;

  word batteryStatus = BatteryStatus();  // 12.1.6 0x0A/0B BatteryStatus()
  word manufacturingStatus = ManufacturingStatus();  // 12.2.33 AltManufacturerAccess() 0x0057 ManufacturingStatus
  u32 operationStatus = OperationStatus();  // 12.2.30 AltManufacturerAccess() 0x0054 OperationStatus
  u32 safetyStatus = SafetyStatus();  // 12.2.27 AltManufacturerAccess() 0x0051 SafetyStatus
  u32 safetyAlert = SafetyAlert();  // 12.2.26 AltManufacturerAccess() 0x0050 SafetyAlert
  word controlRegister = ManufacturerAccessControl();  // 12.1.1 0x00/01 ManufacturerAccess() Control
  int chargingCurrent = ChargingCurrent();  // 12.1.26 0x32/33 ChargingCurrent()
  float chargingVoltage = ChargingVoltage();  // 12.1.25 0x30/31 ChargingVoltage()
  float fullChargeCapacity = FullChargeCapacity();  // 12.1.10 0x12/13 FullChargeCapacity()
  float designCapacity = DesignCapacity();  // 12.1.27 0x3C/3D DesignCapacity()

  Serial.println("batteryStatus: " + String(batteryStatus));
  Serial.println("manufacturingStatus: " + String(manufacturingStatus));
  Serial.println("operationStatus: " + String(operationStatus));
  Serial.println("safetyStatus: " + String(safetyStatus));
  Serial.println("safetyAlert: " + String(safetyAlert));
  Serial.println("controlRegister: " + String(controlRegister));
  Serial.println("chargingCurrent: " + String(chargingCurrent));
  Serial.println("chargingVoltage: " + String(chargingVoltage));
  Serial.println("fullChargeCapacity: " + String(fullChargeCapacity));
  Serial.println("designCapacity: " + String(designCapacity));
  
  PGM_PRINTLN("\nDONE #####################\n");
}

void loop() {
}
