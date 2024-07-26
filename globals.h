/**
  @file globals.h

  @brief Global constants

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

#include "flags.h"

extern bool
/**
  @brief Do not print data into Serial from the functions.
*/
SILENCE,
/**
  @brief Print extended info.
*/
DEBUG;

const char START_MESSAGE[] PROGMEM = "\n============ START: [BQ28Z610 Driver] ============\nLicense: MIT License\nCopyright (c) 2024 Oleksii Sylichenko\n==================================================\n";

/**
  @brief Standard BQ28Z610 Device I2C Address
  @see 12.1 Standard Data Commands
*/
const int DEVICE_ADDR = 0x55;

/**
  @brief Constants for securityMode() result
*/
class SecurityMode {
  public:
    /**
      (SEC1, SEC0) = (0, 1)
    */
    static const byte FULL_ACCESS = 1;
    /**
      (SEC1, SEC0) = (1, 0)
    */
    static const byte UNSEALED = 2;
    /**
      (SEC1, SEC0) = (1, 1)
    */
    static const byte SEALED = 3;
};

/**
  @brief Chapter 9 Device Security

  @see 12.2.23 AltManufacturerAccess() 0x0035 Security Keys
  @see unsealDevice()
*/
class DeviceSecurity {
  public:
    /**
      @brief BQ28Z610 default Unseal key
    */
    static const u32 DEFAULT_UNSEAL_KEY = 0x36720414;
    /**
      @brief BQ28Z610 default Full Access key.

      @warning [!] Checking Full Acces Security Mode:
                   - 12.1.1 0x00/01 ManufacturerAccessControl() - responses [SEC1, SEC0] (Bits 14, 13) = (0, 0) - which is WRONG!
                   - 12.2.30 AltManufacturerAccess() 0x0054 OperationStatus() - responses [SEC1, SEC0] (Bits 9–8) = (0, 1) - which is CORRECT!

      @warning [!] Can be used only for Unsealed Device (after using Unseal Key).
    */
    static const u32 DEFAULT_FULL_ACCESS_KEY = 0xFFFFFFFF;
};

/**
  @brief Block Protocol contants

  - The first 2 bytes are the request address
  - Request payload max size = 32
  - Checksum
  - Request length
*/
class BlockProtocol {
  public:
    static const int REQUEST_MAX_SIZE = 32;  ///< The maximum number of bytes that can be obtained from the device in a single request.
    static const int PAYLOAD_MAX_SIZE = REQUEST_MAX_SIZE;  ///< The maximum number of data bytes that can be obtained from the device in a single function call.

    static const int ADDR_SIZE = 2;  ///< Number of bytes for address, 2.
    static const int CHECKSUM_SIZE = 1;  ///< Number of bytes for Checksum, 1.
    static const int LENGTH_SIZE = 1;  ///< Number of bytes for Length, 1.
    static const int CHECKSUM_AND_LENGTH_SIZE = CHECKSUM_SIZE + LENGTH_SIZE;  ///< Number of bytes for the Checksum and Length, 2.
    static const int SERVICE_SIZE = ADDR_SIZE + CHECKSUM_SIZE + LENGTH_SIZE;  ///< Number of the service bytes (address, checksum, length), 4.

    static const int DATA_INDEX = 2;  ///< Index of the byte from which data starts.
    static const int CHECKSUM_INDEX = 34;  ///< The index of the byte that stores the checksum of the response.
    static const int LENGTH_INDEX = 35;  ///< The index of the byte that stores the total length of the response.

    static const int RESPONSE_MAX_SIZE = 36;  ///< Maximum size of the full response in bytes.
    static const int MAX = RESPONSE_MAX_SIZE;
};

/**
  @brief 12.1 Standard Data Commands

  To enable the system to read and write battery information,
  the BQ28Z610 uses a series of 2-byte standard I2C commands.
*/
class StdCommands {
  public:
    /**
      @see 12.1.1 0x00/01 ManufacturerAccessControl()
    */
    static const byte MANUFACTURER_ACCESS_CONTROL = 0x00;
    /**
      @see 12.1.4 0x06/07 Temperature()
    */
    static const byte TEMPERATURE = 0x06;
    /**
      @see 12.1.5 0x08/09 Voltage()
    */
    static const byte VOLTAGE = 0x08;
    /**
      @see 12.1.6 0x0A/0B BatteryStatus()
    */
    static const byte BATTERY_STATUS = 0x0A;
    /**
      @see 12.1.7 0x0C/0D Current()
    */
    static const byte CURRENT = 0x0C;
    /**
      @see 12.1.9 0x10/11 RemainingCapacity()
    */
    static const byte REMAINING_CAPACITY = 0x10;
    /**
      @see 12.1.10 0x12/13 FullChargeCapacity()
    */
    static const byte FULL_CHARGE_CAPACITY = 0x12;
    /**
      @see 12.1.11 0x14/15 AverageCurrent()
    */
    static const byte AVERAGE_CURRENT = 0x14;
    /**
      @see 12.1.22 0x2A/2B CycleCount()
    */
    static const byte CYCLE_COUNT = 0x2A;
    /**
      @see 12.1.23 0x2C/2D RelativeStateOfCharge()
    */
    static const byte RELATIVE_STATE_OF_CHARGE = 0x2C;
    /**
      @see 12.1.24 0x2E/2F State-of-Health (SOH)
    */
    static const byte STATE_OF_HEALTH = 0x2E;
    /**
      @see 12.1.25 0x30/31 ChargingVoltage()
    */
    static const byte CHARGING_VOLTAGE = 0x30;
    /**
      @see 12.1.26 0x32/33 ChargingCurrent()
    */
    static const byte CHARGING_CURRENT = 0x32;
    /**
      @see 12.1.27 0x3C/3D DesignCapacity()
    */
    static const byte DESIGN_CAPACITY = 0x3C;
    /**
      @see 12.1.28 0x3E/3F AltManufacturerAccess()
    */
    static const byte ALT_MANUFACTURER_ACCESS = 0x3E;
    /**
      @see 12.1.29 0x40/0x5F MACData()
    */
    static const byte MAC_DATA = 0x40;
    /**
      Checksum and lendgth start address:
      - 12.1.30 0x60 MACDataChecksum()
      - 12.1.31 0x61 MACDataLength()
    */
    static const byte MAC_DATA_CHECKSUM = 0x60;
};

/**
  @brief 12.2 0x3E, 0x3F AltManufacturerAccess() Commands
*/
class AltManufacturerCommands {
  public:
    /**
      @see 12.2.1 AltManufacturerAccess() 0x0001 DeviceType()
    */
    static const word DEVICE_TYPE = 0x0001;
    /**
      @see 12.2.2 AltManufacturerAccess() 0x0002 FirmwareVersion()
    */
    static const word FIRMWARE_VERSION = 0x0002;
    /**
      @see 12.2.3 AltManufacturerAccess() 0x0003 HardwareVersion()
    */
    static const word HARDWARE_VERSION = 0x0003;
    /**
      @see 12.2.6 AltManufacturerAccess() 0x0006 ChemicalID()
    */
    static const word CHEMICAL_ID = 0x0006;
    /**
      @see 12.2.12 AltManufacturerAccess() 0x0012 DeviceReset()
    */
    static const word DEVICE_RESET = 0x0012;
    /**
      @see 12.2.13 AltManufacturerAccess() 0x001F ChargeFET()
    */
    static const word CHG_FET = 0x001F;
    /**
      @see 12.2.14 AltManufacturerAccess() 0x0020 DischargeFET()
    */
    static const word DSG_FET = 0x0020;
    /**
      @see 12.2.15 AltManufacturerAccess() 0x0021 Gauging()
    */
    static const word GAUGE_EN = 0x0021;
    /**
      @see 12.2.16 AltManufacturerAccess() 0x0022 FETControl()
    */
    static const word FET_CONTROL = 0x0022;
    /**
      @see 12.2.19 AltManufacturerAccess() 0x0028 Lifetime Data Reset
    */
    static const word LIFETIME_DATA_RESET = 0x0028;
    /**
      @see 12.2.20 AltManufacturerAccess() 0x0029 PermanentFailureDataReset()
    */
    static const word PERMANENT_FAIL_DATA_RESET = 0x0029;
    /**
      @see 12.2.22 AltManufacturerAccess() 0x0030 SealDevice()
    */
    static const word SEAL_DEVICE = 0x0030;
    /**
      @see 12.2.26 AltManufacturerAccess() 0x0050 SafetyAlert()
    */
    static const word SAFETY_ALERT = 0x0050;
    /**
      @see 12.2.28 AltManufacturerAccess() 0x0052 PFAlert()
    */
    static const word PF_ALERT = 0x0052;
    /**
      @see 12.2.29 AltManufacturerAccess() 0x0053 PFStatus()
    */
    static const word PF_STATUS = 0x0053;
    /**
      @see 12.2.30 AltManufacturerAccess() 0x0054 OperationStatus()
    */
    static const word OPERATION_STATUS = 0x0054;
    /**
      @see 12.2.31 AltManufacturerAccess() 0x0055 ChargingStatus()
    */
    static const word CHARGING_STATUS = 0x0055;
    /**
      @see 12.2.32 AltManufacturerAccess() 0x0056 GaugingStatus()
    */
    static const word GAUGING_STATUS = 0x0056;
    /**
      @see 12.2.33 AltManufacturerAccess() 0x0057 ManufacturingStatus()
    */
    static const word MANUFACTURER_STATUS = 0x0057;
    /**
      @see 12.2.37 AltManufacturerAccess() 0x0071 DAStatus1()
    */
    static const word DA_STATUS_1 = 0x0071;
    /**
      @see 12.2.38 AltManufacturerAccess() 0x0072 DAStatus2()
    */
    static const word DA_STATUS_2 = 0x0072;
    /**
      @see 12.2.39 AltManufacturerAccess() 0x0073 ITStatus1()
    */
    static const word IT_STATUS_1 = 0x0073;
    /**
      @see 12.2.40 AltManufacturerAccess() 0x0074 ITStatus2()
    */
    static const word IT_STATUS_2 = 0x0074;
    /**
      @see 12.2.41 AltManufacturerAccess() 0x0075 ITStatus3()
    */
    static const word IT_STATUS_3 = 0x0075;
};

/**
  @brief 12.2.37 AltManufacturerAccess() 0x0071 DAStatus1()

  Indexes of the output data.
*/
class DA_STATUS_1 {
  public:
    /**
      AAaa: Cell Voltage 1
    */
    static const byte CELL_VOLTAGE_1 = 0;
    /**
      BBbb: Cell Voltage 2
    */
    static const byte CELL_VOLTAGE_2 = 2;
    /**
      EEee: BAT Voltage
    */
    static const byte BAT_VOLTAGE = 8;
    /**
      FFff: PACK Voltage
    */
    static const byte PACK_VOLTAGE = 10;
    /**
      GGgg: Cell Current 1. Simultaneous current measured during Cell Voltage1 measurement
    */
    static const byte CELL_CURRENT_1 = 12;
    /**
      HHhh: Cell Current 2. Simultaneous current measured during Cell Voltage2 measurement
    */
    static const byte CELL_CURRENT_2 = 14;
    /**
      KKkk: Cell Power 1. Calculated using Cell Voltage1 and Cell Current 1 data
    */
    static const byte CELL_POWER_1 = 20;
    /**
      LLll: Cell Power 2. Calculated using Cell Voltage2 and Cell Current 2 data
    */
    static const byte CELL_POWER_2 = 22;
    /**
      OOoo: Power calculated by Voltage() × Current()
    */
    static const byte POWER = 28;
    /**
      PPpp: Average Power. Calculated by Voltage() × AverageCurrent()
    */
    static const byte AVG_POWER = 30;
};

/**
  @brief 12.2.40 AltManufacturerAccess() 0x0074 ITStatus2()

  Indexes of the output data.
*/
class IT_STATUS_2 {
  public:
    /**
      KKkk: DOD0 Passed Q. Passed charge since DOD0
    */
    static const byte DOD0_PASSED_Q = 14;
};

/**
  @brief 12.2.41 AltManufacturerAccess() 0x0075 ITStatus3()

  Indexes of the output data.
*/
class IT_STATUS_3 {
  public:
    /**
      AAaa: QMax 1. QMax of Cell 1
    */
    static const byte QMax_1 = 0;
    /**
      BBbb: QMax 2. QMax of Cell 2
    */
    static const byte QMax_2 = 2;
    /**
      CCcc: QMaxDOD0_1. Cell 1 DOD for Qmax
    */
    static const byte QMaxDOD0_1 = 4;
    /**
      DDdd: QMaxDOD0_2. Cell 2 DOD for Qmax
    */
    static const byte QMaxDOD0_2 = 6;
    /**
      EEee: QMaxPassedQ. Passed charge since DOD for Qmax recorded (mAh)
    */
    static const byte QMaxPassedQ = 8;
    /**
      FFff: QMaxTime. Time since DOD for Qmax recorded (hour / 16 units)
    */
    static const byte QMaxTime = 10;
    /**
      GGgg: Tk. Thermal model “k”
    */
    static const byte Tk = 12;
    /**
      HHhh: Ta. Thermal model “a”
    */
    static const byte Ta = 14;
    /**
      IIii: RawDOD0_1. Cell 1 raw DOD0 measurement
    */
    static const byte RAW_DOD0_1 = 16;
    /**
      JJjj: RawDOD0_2. Cell 2 raw DOD0 measurement
    */
    static const byte RAW_DOD0_2 = 18;
};

/**
  @brief Units of measurement to print to serial port.
*/
class Units {
  public:
    static PGM_P MAH() {
      return PSTR("mAh");
    }
    static PGM_P CWH() {
      return PSTR("cWh");
    }
    static PGM_P MW() {
      return PSTR("mW");
    }
    static PGM_P MA() {
      return PSTR("mA");
    }
    static PGM_P V() {
      return PSTR("V");
    }
    static PGM_P PERCENT() {
      return PSTR("%");
    }
    static PGM_P CELSIUS() {
      return PSTR("°C");
    }
};

/**
  @brief 1/10 part
*/
const float DECIPART = 0.1;
/**
  @brief Number of decimal places
*/
const float DECIPART_DECIMAL = 1;

/**
  @brief 1/1000 part
*/
const float PERMIL = 0.001;
/**
  @brief Number of decimal places
*/
const byte PERMIL_DECIMAL = 3;
