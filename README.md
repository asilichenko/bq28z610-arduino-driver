# Custom Arduino Driver for Battery Gas Gauging Device BQ28Z610

<img width="600" alt="Arduino BQ28Z610 Driver" src="https://github.com/user-attachments/assets/92a0b1af-471d-4496-8bc8-e6f9120a4c4d"/>

This is a driver for Arduino that provides functions to interface with the BQ28Z610 battery gas gauge device over I2C protocol.

With this driver, you can operate the BQ28Z610 device via the serial port from your computer using Arduino.

Functions were implemented based on the documentation:

* Technical Reference Manual (c) Texas Instruments, Literature Number: SLUUA65E, https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf

Not all functions described in the Technical Reference Manual were implemented, but most of them.

The driver also includes some additional service functions for more usability.

# Project Files

- [bq28z610-arduino-driver.ino](#-bq28z610-arduino-driverino)
- [doxyfile](#-doxyfile)
- [std_data_commands](std_data_commands)
- [alt_manufacturer_access](#-alt_manufacturer_access)
- [data_flash_access](#-data_flash_access)
- [service](#-service)
- [utils](#-utils)
- [flags.h](#-flagsh)
- [globals.h](#-globalsh)
- /extras/data_flash/:
  - [data_flash.py](#-data_flashpy)
  - [data/data_descriptions.csv](#-data_descriptionscsv)
  - [dump files](#-dump-files)

Next, I will provide a brief description of the file contents.

For a complete description of all functions, follow the link: https://asilichenko.github.io/bq28z610-arduino-driver/files.html

## 📄 bq28z610-arduino-driver.ino

The main Arduino sketch file.

1. Wait 5 seconds to prevent launching the code when resetting while uploading new sketch to Arduino.
2. Init Serial port.
3. Init Wire (I2C) interface.

There is some sample code in the setup function:

- Calling "read and print" functions from different sections.
- Example of using the SILENCE constant and printing the results of the called functions.

An example of the output result in the serial port from executing this code can be viewed at the link: [example-output.txt](extras/example-output.txt)

🔗 [bq28z610-arduino-driver.ino](bq28z610-arduino-driver.ino)

## 📄 doxyfile

This file describes the settings to be used by the documentation system doxygen (www.doxygen.org) for a project.

Output directory for the generated documentation is "[/docs/](docs)".

🔗 [doxyfile](doxyfile)

## 📄 std_data_commands

Functions described in the section "_12.1 Standard Data Commands_" of the Technical Reference Manual.

🔗 [std_data_commands.h](std_data_commands.h) | [std_data_commands.cpp](std_data_commands.cpp)

## 📄 alt_manufacturer_access

Functions described in the section "_12.2 0x3E, 0x3F AltManufacturerAccess()_" of the Technical Reference Manual.

🔗 [alt_manufacturer_access.h](alt_manufacturer_access.h) | [alt_manufacturer_access.cpp](alt_manufacturer_access.cpp)

## 📄 data_flash_access

Functions to read data from the Data Flash and write to the Data Flash, according to:

- 12.2.45 Data Flash Access() 0x4000–0x5FFF
- 13 Data Flash Values
- 14 Data Flash Summary

To access the data flash, your device needs to be in an UNSEALED state (UNSEALED is enough; Full Access state is not necessary).

The default Unseal Key is defined in the class DeviceSecurity in the file [globals.h](globals.h).

Contains such constants and functions as:

- Data Flash addresses constants
- Read and write single byte, word and array (maximum 32 bytes)
- Read data in the String format
- Read and write values for named Data Flash data
- Print Ra Table
- Reset Ra Table flags
- Print Data Flash dump

The Technical Reference Manual only contains descriptions up to address 0x4727, but there is some data in the rest of the Data Flash.

Some addresses within the standard region are prohibited from writing, although I am not sure about the exact ranges of these addresses. For example, around the address 0x5A5A, there is no data (bytes are 0xFF) and they cannot be overwritten.

🔗 [data_flash_access.h](data_flash_access.h) | [data_flash_access.cpp](data_flash_access.cpp)

## 📄 service

Additional service functions for the device, like:

- Unsealing the device
- Check if the device is in the Permanent Fail state
- Get current security mode
- Put Charge or Discharge FET into the specific state
- Get specific data from raw-data commands like DAStatus and ITStatus
-  Print current state of the flags important for some protections (Undervoltage, Overtemperature, Short circuit in charge and discharge, etc.)
- Print flags that correspond to the FETs status, to determine whether they are enabled and why
- Enable or Disable turning charging FET off at 60% SOC (see: https://www.linkedin.com/pulse/how-stop-battery-charging-specific-percentage-oleksii-sylichenko-vpmkf)
- Init the device before the Learning Cycle (see: https://www.linkedin.com/pulse/gas-gauging-device-bq28z610-learning-cycle-practical-guide-oleksii-ngk8f)
- Print information important to be checked periodically during the learning cycle (see: https://www.linkedin.com/pulse/gas-gauging-device-bq28z610-learning-cycle-practical-guide-oleksii-ngk8f)
- Get and set some thresholds

🔗 [service.h](service.h) | [service.cpp](service.cpp)

## 📄 utils

Util functions for:

- Printing into serial port
- Composing full values from bytes
- Sending and receiving data via I2C protocol
- Implementation of the high-level Block Protocol of the device

🔗 [utils.h](utils.h) | [utils.cpp](utils.cpp)

## 📄 flags.h

Definition of the flag constants with the descriptions.

🔗 [flags.h](flags.h)

## 📄 globals.h

Definition of the global constants except flags, like:

- I2C address of the Device
- Security modes
- Unseal keys
- Block protocol parameters
- Codes of the "_12.1 Standard Data Commands_"
- Codes of the "_12.2 0x3E, 0x3F Alt Manufacturer Access Commands_"
- Constants for some raw data commands like DAStatus and ITStatus
- Strings that represent units of measurement

🔗 [globals.h](globals.h)

## 📄 data_flash.py

Python script for processing the Data Flash dump, printed by the function from the data_flash_access:

- Load from the text file
- Print data detailed - values with addresses and captions from the "_14.1 Data Flash Table_" of the Technical Manual
- Compare two dumps and print the differences

🔗 [data_flash.py](extras/data_flash/data_flash.py)

## 📄 data_descriptions.csv

Csv-file which contains list of Data Flash entities taken from the table "_14.1 Data Flash Table_":

- Physical address
- Data format (I4, U2, H1, F4, S5, etc.) that represents column "Type"
- Description which is a join of the columns: "Class", "Subclass" and "Name" separated by "/" symbol

🔗 [data_descriptions.csv](extras/data_flash/data/data_descriptions.csv)

## 📂 dump files

I included several Data Flash dump files of my BQ28Z610 device:

- [dump-original.txt](extras/data_flash/data/dump/dump-original.txt) - a dump before any changes over Data Flash were made
- [dump-20240710.txt](extras/data_flash/data/dump/dump-20240710.txt) - a dump after some changes have been written to the Data Flash
- [dump-20240710-after-lifetime-reset.txt](extras/data_flash/data/dump/dump-20240710-after-lifetime-reset.txt) - a dump to check how the function "_12.2.19 AltManufacturerAccess() 0x0028 Lifetime Data Reset_" works
- [dump-original-detailed.txt](extras/data_flash/data/output/dump-original-detailed.txt) - the result of detailing a raw dump

# Arduino Nano Connection

- GND -> black
- A4 = SDA -> white
- A5 = SCL -> yellow

# Author
[@asilichenko](https://github.com/asilichenko)

# License
[MIT license](LICENSE)

# References

- Texas Instruments BQ28Z610 Technical Reference Manual: https://www.ti.com/lit/ug/sluua65e/sluua65e.pdf
- BQ28Z610 product: https://www.ti.com/product/BQ28Z610
- Texas Instruments Power Management Forum: https://e2e.ti.com/support/power-management-group/power-management/f/power-management-forum
- How to Stop Battery Charging at a Specific Percentage: https://www.linkedin.com/pulse/how-stop-battery-charging-specific-percentage-oleksii-sylichenko-vpmkf
- Gas Gauging Device BQ28Z610 Learning Cycle: Practical Guide: https://www.linkedin.com/pulse/gas-gauging-device-bq28z610-learning-cycle-practical-guide-oleksii-ngk8f
- Doxygen, documentation generator tool: https://www.doxygen.nl

# Sources

- [bq28z610-arduino-driver.ino](bq28z610-arduino-driver.ino)
- [doxyfile](doxyfile)
- [std_data_commands.h](std_data_commands.h) | [std_data_commands.cpp](std_data_commands.cpp)
- [alt_manufacturer_access.h](alt_manufacturer_access.h) | [alt_manufacturer_access.cpp](alt_manufacturer_access.cpp)
- [data_flash_access.h](data_flash_access.h) | [data_flash_access.cpp](data_flash_access.cpp)
- [service.h](service.h) | [service.cpp](service.cpp)
- [utils.h](utils.h) | [utils.cpp](utils.cpp)
- [flags.h](flags.h)
- [globals.h](globals.h)
- [data_flash.py](extras/data_flash/data_flash.py)
- Documentation generated by Doxygen: https://asilichenko.github.io/bq28z610-arduino-driver/
