/**
  @file flags.h

  @brief Flags constants

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

/**
  @brief Structure that stores flag information.
*/
struct Flag {

  /**
    Bit index.
  */
  byte n;

  /**
    Caption of the flag from the documentation.
  */
  const char *caption;
};

/**
  @brief 12.1.1 0x00/01 ManufacturerAccessControl()

  A read on this register returns the Control bits.

  This Control Register is an I2C register, and the control bits are read back on register 0x00/0x01.
  These control bits are provided for backward compatibility/ease-of-use.
*/
class ManufacturerAccessFlags {
  public:
    static Flag SEC1() {
      return {14, PSTR("SEC1 (Bit 14): SECURITY Mode")};
    }
    static Flag SEC0() {
      return {13, PSTR("SEC0 (Bit 13): SECURITY Mode")};
    }
    static Flag AUTHCALM() {
      return {12, PSTR("AUTHCALM (Bit 12): Automatic CALIBRATION mode")};
    }
    static Flag CheckSumValid() {
      return {9, PSTR("CheckSumValid (Bit 9): Checksum Valid")};
    }
    static Flag BTP_INT() {
      return {7, PSTR("BTP_INT (Bit 7): Battery Trip Point Interrupt")};
    }
    static Flag LDMD() {
      return {3, PSTR("LDMD (Bit 3): LOAD Mode")};
    }
    static Flag R_DIS() {
      return {2, PSTR("R_DIS (Bit 2): Resistance Updates")};
    }
    static Flag VOK() {
      return {1, PSTR("VOK (Bit 1): Voltage OK for QMax Update")};
    }
    static Flag QMax() {
      return {0, PSTR("QMax (Bit 0): QMax Updates. This bit toggles after every QMax update")};
    }
};

/**
  @brief 12.1.6 0x0A/0B BatteryStatus()
*/
class BatteryStatusFlags {
  public:
    static const byte ERR_CODE = 0b111;
    static Flag EC0() {
      return {0, PSTR("EC0 (Bit 0): Error Code")};
    }
    static Flag EC1() {
      return {1, PSTR("EC1 (Bit 1): Error Code")};
    }
    static Flag EC2() {
      return {2, PSTR("EC2 (Bit 2): Error Code")};
    }
    static Flag EC3() {
      return {3, PSTR("EC3 (Bit 3): Error Code")};
    }
    static Flag FD() {
      return {4, PSTR("FD (Bit 4): Fully Discharged: [Battery ok; Battery fully depleted]")};
    }
    static Flag FC() {
      return {5, PSTR("FC (Bit 5): Fully Charged: [Battery not fully charged; Battery fully charged]")};
    }
    static Flag DSG() {
      return {6, PSTR("DSG (Bit 6): Discharging: [Battery is charging; Battery is discharging]")};
    }
    static Flag INIT() {
      return {7, PSTR("INIT (Bit 7): Initialization: [Inactive; Active]")};
    }
    static Flag RTA() {
      return {8, PSTR("RTA (Bit 8): Remaining Time Alarm: [Inactive; Active]")};
    }
    static Flag RCA() {
      return {9, PSTR("RCA (Bit 9): Remaining Capacity Alarm: [Inactive; Active]")};
    }
    static Flag TDA() {
      return {11, PSTR("TDA (Bit 11): Terminate Discharge Alarm: [Inactive; Active]")};
    }
    static Flag OTA() {
      return {12, PSTR("OTA (Bit 12): Overtemperature Alarm: [Inactive; Active]")};
    }
    static Flag TCA() {
      return {14, PSTR("TCA (Bit 14): Terminate Charge Alarm: [Inactive; Active]")};
    }
    static Flag OCA() {
      return {15, PSTR("OCA (Bit 15): Overcharged Alarm: [Inactive; Active]")};
    }
};

/**
  @brief 12.2.26 AltManufacturerAccess() 0x0050 SafetyAlert()
*/
class SafetyAlertFlags {
  public:
    static Flag DSG() {
      return {27, PSTR("UTD (Bit 27): Undertemperature During Discharge")};
    }
    static Flag UTC() {
      return {26, PSTR("UTC (Bit 26): Undertemperature During Charge")};
    }
    static Flag CTOS() {
      return {21, PSTR("CTOS (Bit 21): Charge Timeout Suspend")};
    }
    static Flag PTOS() {
      return {19, PSTR("PTOS (Bit 19): Precharge Timeout Suspend")};
    }
    static Flag OTD() {
      return {13, PSTR("OTD (Bit 13): Overtemperature During Discharge")};
    }
    static Flag OTC() {
      return {12, PSTR("OTC (Bit 12): Overtemperature During Charge")};
    }
    static Flag ASCD() {
      return {10, PSTR("ASCD (Bit 10): Short-Circuit During Discharge")};
    }
    static Flag ASCC() {
      return {8, PSTR("ASCC (Bit 8): Short-Circuit During Charge")};
    }
    static Flag AOLD() {
      return {6, PSTR("AOLD (Bit 6): Overload During Discharge")};
    }
    static Flag OCD() {
      return {4, PSTR("OCD (Bit 4): Overcurrent During Discharge")};
    }
    static Flag OCC() {
      return {2, PSTR("OCC (Bit 2): Overcurrent During Charge")};
    }
    static Flag COV() {
      return {1, PSTR("COV (Bit 1): Cell Overvoltage")};
    }
    static Flag CUV() {
      return {0, PSTR("CUV (Bit 0): Cell Undervoltage")};
    }
};

/**
  @brief 12.2.27 AltManufacturerAccess() 0x0051 SafetyStatus()
*/
class SafetyStatusFlags {
  public:
    static Flag UTD() {
      return {27, PSTR("UTD (Bit 27): Undertemperature During Discharge")};
    }
    static Flag UTC() {
      return {26, PSTR("UTC (Bit 26): Undertemperature During Charge")};
    }
    static Flag CTO() {
      return {20, PSTR("CTO (Bit 20): Charge Timeout")};
    }
    static Flag PTO() {
      return {18, PSTR("PTO (Bit 18): Precharge Timeout")};
    }
    static Flag OTD() {
      return {13, PSTR("OTD (Bit 13): Overtemperature During Discharge")};
    }
    static Flag OTC() {
      return {12, PSTR("OTC (Bit 12): Overtemperature During Charge")};
    }
    static Flag ASCD() {
      return {10, PSTR("ASCD (Bit 10): Short-Circuit During Discharge")};
    }
    static Flag ASCC() {
      return {8, PSTR("ASCC (Bit 8): Short-Circuit During Charge")};
    }
    static Flag AOLD() {
      return {6, PSTR("AOLD (Bit 6): Overload During Discharge")};
    }
    static Flag OCD() {
      return {4, PSTR("OCD (Bit 4): Overcurrent During Discharge")};
    }
    static Flag OCC() {
      return {2, PSTR("OCC (Bit 2): Overcurrent During Charge")};
    }
    static Flag COV() {
      return {1, PSTR("COV (Bit 1): Cell Overvoltage")};
    }
    static Flag CUV() {
      return {0, PSTR("CUV (Bit 0): Cell Undervoltage")};
    }
};

/**
  @brief 12.2.29 AltManufacturerAccess() 0x0053 PFStatus()
*/
class PFStatusFlags {
  public:
    static Flag DFW() {
      return {26, PSTR("DFW (Bit 26): Data Flash Wearout Failure")};
    }
    static Flag IFC() {
      return {24, PSTR("IFC (Bit 24): Instruction Flash Checksum Failure")};
    }
    static Flag DFETF() {
      return {17, PSTR("DFETF (Bit 17): Discharge FET Failure")};
    }
    static Flag CFETF() {
      return {16, PSTR("CFETF (Bit 16): Charge FET Failure")};
    }
    static Flag VIMR() {
      return {12, PSTR("VIMR (Bit 12): Voltage Imbalance While Pack Is At Rest Failure")};
    }
    static Flag VIMA() {
      return {11, PSTR("VIMA (Bit 11): Voltage Imbalance While Pack Is Active Failure")};
    }
    static Flag SOV() {
      return {1, PSTR("SOV (Bit 1): Safety Cell Overvoltage Failure")};
    }
};

/**
  @brief 12.2.30 AltManufacturerAccess() 0x0054 OperationStatus()
*/
class OperationStatusFlags {
  public:
    static Flag EMSHUT() {
      return {29, PSTR("EMSHUT (Bit 29): Emergency FET Shutdown")};
    }
    static Flag CB() {
      return {28, PSTR("CB (Bit 28): Cell Balancing")};
    }
    static Flag SLPCC() {
      return {27, PSTR("SLPCC (Bit 27): CC Measurement in SLEEP mode")};
    }
    static Flag SLPAD() {
      return {26, PSTR("SLPAD (Bit 26): ADC Measurement in SLEEP mode")};
    }
    static Flag SMBLCAL() {
      return {25, PSTR("SMBLCAL (Bit 25): Auto-offset calibration when Bus low is detected")};
    }
    static Flag INIT() {
      return {24, PSTR("INIT (Bit 24): Initialization after full reset")};
    }
    static Flag SLEEPM() {
      return {23, PSTR("SLEEPM (Bit 23): SLEEP mode")};
    }
    static Flag XL() {
      return {22, PSTR("XL (Bit 22): 400-kHz mode")};
    }
    static Flag CAL_OFFSET() {
      return {21, PSTR("CAL_OFFSET (Bit 21): Calibration Output (raw CC Offset data)")};
    }
    static Flag CAL() {
      return {20, PSTR("CAL (Bit 20): Calibration Output (raw ADC and CC data)")};
    }
    static Flag AUTHCALM() {
      return {19, PSTR("AUTHCALM (Bit 19): Auto CC Offset Calibration by MAC AutoCCOffset")};
    }
    static Flag AUTH() {
      return {18, PSTR("AUTH (Bit 18): Authentication in progress")};
    }
    static Flag SDM() {
      return {16, PSTR("SDM (Bit 16): SHUTDOWN triggered via command")};
    }

    // =====

    static Flag SLEEP() {
      return {15, PSTR("SLEEP (Bit 15): SLEEP mode conditions met")};
    }
    static Flag XCHG() {
      return {14, PSTR("XCHG (Bit 14): Charging disabled")};
    }
    static Flag XDSG() {
      return {13, PSTR("XDSG (Bit 13): Discharging disabled")};
    }
    static Flag PF() {
      return {12, PSTR("PF (Bit 12): PERMANENT FAILURE mode status")};
    }
    static Flag SS() {
      return {11, PSTR("SS (Bit 11): SAFETY mode status")};
    }
    static Flag SDV() {
      return {10, PSTR("SDV (Bit 10): SHUTDOWN triggered via low pack voltage")};
    }
    static Flag SEC1() {
      return {9, PSTR("SEC1 (Bit 9)")};
    }
    static Flag SEC0() {
      return {8, PSTR("SEC0 (Bit 8)")};
    }
    static Flag BTP_INT() {
      return {7, PSTR("BTP_INT (Bit 7): Battery Trip Point (BTP) Interrupt output")};
    }
    static Flag CHG() {
      return {2, PSTR("CHG (Bit 2): CHG FET status")};
    }
    static Flag DSG() {
      return {1, PSTR("DSG (Bit 1): DSG FET status")};
    }
};

/**
  @brief 12.2.31 AltManufacturerAccess() 0x0055 ChargingStatus()
*/
class ChargingStatusFlags {
  public:
    static Flag VCT() {
      return {15, PSTR("VCT (Bit 15): Charge Termination")};
    }
    static Flag MCHG() {
      return {14, PSTR("MCHG (Bit 14): Maintenance Charge")};
    }
    static Flag SU() {
      return {13, PSTR("SU (Bit 13): Charge Suspend")};
    }
    static Flag IN() {
      return {12, PSTR("IN (Bit 12): Charge Inhibit")};
    }
    static Flag HV() {
      return {11, PSTR("HV (Bit 11): High Voltage Region")};
    }
    static Flag MV() {
      return {10, PSTR("MV (Bit 10): Mid Voltage Region")};
    }
    static Flag LV() {
      return {9, PSTR("LV (Bit 9): Low Voltage Region")};
    }
    static Flag PV() {
      return {8, PSTR("PV (Bit 8): Precharge Voltage Region")};
    }
    static Flag OT() {
      return {6, PSTR("OT (Bit 6): Over Temperature Region")};
    }
    static Flag HT() {
      return {5, PSTR("HT (Bit 5): High Temperature Region")};
    }
    static Flag STH() {
      return {4, PSTR("STH (Bit 4): Standard Temperature High Region")};
    }
    static Flag RT() {
      return {3, PSTR("RT (Bit 3): Room Temperature Region")};
    }
    static Flag STL() {
      return {2, PSTR("STL (Bit 2): Standard Temperature Low Region")};
    }
    static Flag LT() {
      return {1, PSTR("LT (Bit 1): Low Temperature Region")};
    }
    static Flag UT() {
      return {0, PSTR("UT (Bit 0): Under Temperature Region")};
    }
};

/**
  @brief 12.2.32 AltManufacturerAccess() 0x0056 GaugingStatus()
*/
class GaugingStatusFlags {
  public:
    static Flag OCVFR() {
      return {20, PSTR("OCVFR (Bit 20): Open Circuit Voltage in Flat Region (during RELAX)")};
    }
    static Flag LDMD() {
      return {19, PSTR("LDMD (Bit 19): LOAD mode")};
    }
    static Flag RX() {
      return {18, PSTR("RX (Bit 18): Resistance Update (Toggles after every resistance update)")};
    }
    static Flag QMax() {
      return {17, PSTR("QMax (Bit 17): QMax Update (Toggles after every QMax update)")};
    }
    static Flag VDQ() {
      return {16, PSTR("VDQ (Bit 16): Discharge Qualified for Learning (based on RU flag)")};
    }
    static Flag NSFM() {
      return {15, PSTR("NSFM (Bit 15): Negative Scale Factor Mode")};
    }
    static Flag SLPQMax() {
      return {13, PSTR("SLPQMax (Bit 13): QMax Update During Sleep")};
    }
    static Flag QEN() {
      return {12, PSTR("QEN (Bit 12): Impedance Track Gauging (Ra and QMax updates are enabled)")};
    }
    static Flag VOK() {
      return {11, PSTR("VOK (Bit 11): Voltage OK for QMax Update")};
    }
    static Flag RDIS() {
      return {10, PSTR("RDIS (Bit 10): Resistance Updates")};
    }
    static Flag REST() {
      return {8, PSTR("REST (Bit 8): Rest")};  // documentation contains a typo
    }
    static Flag CF() {
      return {7, PSTR("CF (Bit 7): Condition Flag")};
    }
    static Flag DSG() {
      return {6, PSTR("DSG (Bit 6): Discharge/Relax")};
    }
    static Flag EDV() {
      return {5, PSTR("EDV (Bit 5): End-of-Discharge Termination Voltage")};
    }
    static Flag BAL_EN() {
      return {4, PSTR("BAL_EN (Bit 4): Cell Balancing")};
    }
    static Flag TC() {
      return {3, PSTR("TC (Bit 3): Terminate Charge")};
    }
    static Flag TD() {
      return {2, PSTR("TD (Bit 2): Terminate Discharge")};
    }
    static Flag FC() {
      return {1, PSTR("FC (Bit 1): Fully Charged")};
    }
    static Flag FD() {
      return {0, PSTR("FD (Bit 0): Fully Discharged")};
    }
};

/**
  @brief 12.2.33 AltManufacturerAccess() 0x0057 ManufacturingStatus()
*/
class ManufacturingStatusFlags {
  public:
    static Flag CAL_EN() {
      return {15, PSTR("CAL_EN (Bit 15) CALIBRATION Mode")};
    }
    static Flag PF_EN() {
      return {6, PSTR("PF_EN (Bit 6) Permanent Failure Mode")};
    }
    static Flag LF_EN() {
      return {5, PSTR("LF_EN (Bit 5) Lifetime Data Collection Mode")};
    }
    static Flag FET_EN() {
      return {4, PSTR("FET_EN (Bit 4) All FET Action Mode")};
    }
    static Flag GAUGE_EN() {
      return {3, PSTR("GAUGE_EN (Bit 3) Gas Gauging Mode")};
    }
    static Flag DSG_TEST() {
      return {2, PSTR("DSG_TEST (Bit 2)Discharge FET Test")};
    }
    static Flag CHG_TEST() {
      return {1, PSTR("CHG_TEST (Bit 1) Charge FET Test")};
    }
};

/**
  @brief Data Flash: Settings; Configuration; FET Options; H1
  - JBL: 01011100

  <pre>
  Bit 0: Reserved
  Bit 1: Reserved
  Bit 2: OTFET - FET action in OVERTEMPERATURE mode
    0 = No FET action for overtemperature condition
    1 = CHG and DSG FETs will be turned off for overtemperature conditions.
  Bit 3: CHGSU - FET action in CHARGE SUSPEND mode
    0 = FET active
    1 = Charging or precharging disabled, FET off
  Bit 4: CHGIN - FET action in CHARGE INHIBIT mode
    0 = FET active
    1 = Charging or precharging disabled, FET off
  Bit 5: CHGFET - FET action on valid charge termination
    0 = FET active
    1 = Charging or precharging disabled, FET off
  Bit 6: SLEEPCHG - CHG FET enabled during sleep
    0 = CHG FET off during sleep
    1 = CHG FET remains on during sleep.
  Bit 7: Reserved
  </pre>

  @see dfReadFetOptions()
*/
class FetOptionsFlags {
  public:
    /**
      - 0 = No FET action for overtemperature condition (default)
      - 1 = CHG and DSG FETs will be turned off for overtemperature conditions.
    */
    static Flag OTFET() {
      return {2, PSTR("Bit 2: OTFET - FET action in OVERTEMPERATURE mode")};
    }
    /**
      - 0 = FET active (default)
      - 1 = Charging or precharging disabled, FET off
    */
    static Flag CHGSU() {
      return {3, PSTR("Bit 3: CHGSU - FET action in CHARGE SUSPEND mode")};
    }
    /**
      - 0 = FET active (default)
      - 1 = Charging or precharging disabled, FET off
    */
    static Flag CHGIN() {
      return {4, PSTR("Bit 4: CHGIN - FET action in CHARGE INHIBIT mode")};
    }
    /**
      - 0 = FET active (default)
      - 1 = Charging or precharging disabled, FET off
    */
    static Flag CHGFET() {
      return {5, PSTR("Bit 5: CHGFET - FET action on valid charge termination")};
    }
    /**
      - 0 = CHG FET off during sleep (default)
      - 1 = CHG FET remains on during sleep.
    */
    static Flag SLEEPCHG() {
      return {6, PSTR("Bit 6: SLEEPCHG - CHG FET enabled during sleep")};
    }
};

/**
  @brief Data Flash: Settings; Configuration; 0x469B; DA Configuration; H1

  <pre>
  Bit 0: CC0 - Cell Count
  Bit 1: Reserved
  Bit 2: Reserved
  Bit 3: IN_SYSTEM_SLEEP - In-system SLEEP mode
  Bit 4: SLEEP - SLEEP Mode
  Bit 5: Reserved
  Bit 6: CTEMP - Cell Temperature protection source
  Bit 7: Reserved
  </pre>
*/
class DaConfigurationFlags {
  public:
    /**
      - 0 = 1 cell
      - 1 = 2 cell
    */
    static Flag CC0() {
      return {0, PSTR("Bit 0: CC0—Cell Count")};
    }
    /**
      - 0 = Disables (default)
      - 1 = Enables
    */
    static Flag IN_SYSTEM_SLEEP() {
      return {3, PSTR("Bit 3: IN_SYSTEM_SLEEP—In-system SLEEP mode")};
    }
    /**
      - 0 = Disables SLEEP mode
      - 1 = Enables SLEEP mode (default)
    */
    static Flag SLEEP() {
      return {4, PSTR("Bit 4: SLEEP—SLEEP Mode")};
    }
    /**
      - 0 = MAX (default)
      - 1 = Average
    */
    static Flag CTEMP() {
      return {6, PSTR("Bit 6: CTEMP—Cell Temperature protection source")};
    }
};

/**
  @brief Data Flash: Gas Gauging; State; 0x420E; Update Status; H1
  - Bit 1:0: Update1, Update0 - Update Status
  - Bit 2: Enable - Impedance Track gauging and lifetime updating is enabled.
  - Bit 3: QMax_update - QMax was updated in the field (in real conditions)
  - Bit 7:4: Reserved

  @see dfReadGasGaugingUpdateStatus()
*/
class GasGaugingUpdateStatusFlags {
  public:
    /**
      Bit 1:0: Update1, Update0 - Update Status:
      - 0,0 = Impedance Track gauging and lifetime updating is disabled.
      - 0,1 = QMax updated
      - 1,0 = QMax and Ra table have been updated
    */
    static const byte UPDATE_STATUS = 0b11;
    static Flag Update0() {
      return {0, PSTR("Bit 0: Update0 - Update Status")};
    }
    static Flag Update1() {
      return {1, PSTR("Bit 1: Update0 - Update Status")};
    }
    /**
      - 0 = Disabled
      - 1 = Enabled
    */
    static Flag Enable() {
      return {2, PSTR("Bit 2: Enable - Impedance Track gauging and lifetime updating is enabled")};
    }
    /**
      - 0 = Not updated
      - 1 = Updated
    */
    static Flag QMax_update() {
      return {3, PSTR("Bit 3: QMax_update - QMax was updated in the field (in real conditions)")};
    }
};

/**
  @brief Data Flash: Settings; Configuration; 0x4632; SOC Flag Config A; H2
  - Default: 0x0C8C;
  - JBL: 0001 0011 1000 0011;

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

  @see dfReadSocFlagConfigA()
*/
class SOCFlagConfigAFlags {
  public:
    static Flag TDSETV() {
      return {0, PSTR("Bit 0: TDSETV - Enables the TD flag set by the cell voltage threshold")};
    }
    static Flag TDCLEARV() {
      return {1, PSTR("Bit 1: TDCLEARV - Enables the TD flag clear by cell voltage threshold")};
    }
    static Flag TDSETRSOC() {
      return {2, PSTR("Bit 2: TDSETRSOC - Enables the TD flag set by RSOC threshold")};
    }
    static Flag TDCLEARRSOC() {
      return {3, PSTR("Bit 3: TDCLEARRSOC - Enables the TD flag cleared by the RSOC threshold")};
    }
    static Flag TCSETV() {
      return {4, PSTR("Bit 4: TCSETV - Enables the TC flag set by cell voltage threshold")};
    }
    static Flag TCCLEARV() {
      return {5, PSTR("Bit 5: TCCLEARV - Enables the TC flag clear by cell voltage threshold")};
    }
    static Flag TCSETRSOC() {
      return {6, PSTR("Bit 6: TCSETRSOC - Enables the TC flag set by the RSOC threshold")};
    }
    static Flag TCCLEARRSOC() {
      return {7, PSTR("Bit 7: TCCLEARRSOC - Enables the TC flag cleared by the RSOC threshold")};
    }
    static Flag FCSETVCT() {
      return {10, PSTR("Bit 10: FCSETVCT - Enables the FC flag set by primary charge termination")};
    }
    static Flag TCSETVCT() {
      return {11, PSTR("Bit 11: TCSETVCT - Enables the TC flag set by primary charge termination")};
    }
};
