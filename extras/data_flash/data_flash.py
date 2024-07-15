"""
This script performs Battery Gas Gauging Device BQ28Z610 Data Flash data dump analysis and comparison.

License: MIT License
Copyright (c) 2024 Oleksii Sylichenko

Description:
- Loads data dump from specified input file.
- Processes the data and prints it with the description.
- Compare two dump datasets and prints differences.


MIT License

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
"""

import csv
from struct import unpack

HEX_FORMAT = 16
DATA_DESCRIPTIONS_FILE = 'data/data_descriptions.csv'


class Address:
    START = 0x4000
    END = 0x5FFF

    RA_TABLE_START = 0x4102
    RA_TABLE_END = 0x41DE


FULL_ADDRESSES_RANGE = range(Address.START, Address.END + 1)
ADDRESSES_WITHOUT_RA_TABLE = list(FULL_ADDRESSES_RANGE[:(Address.RA_TABLE_START - Address.START)]) \
                             + list(FULL_ADDRESSES_RANGE[(Address.RA_TABLE_END - Address.START) + 1:])


class DeviceDataFormat:
    """
        13.1 Data Formats

        13.1.1 Unsigned Integer
            U1, U2, U4
            Unsigned integers are stored without changes as 1-byte, 2-byte,
            or 4-byte values in Little Endian byte order.
            - B - One-byte unsigned integer
            - H - Two-byte unsigned integer
            - I - Four-byte unsigned integer

        13.1.2 Integer
            I1, I2, I4
            Integer values are stored in 2's-complement format in 1-byte, 2-byte,
            or 4-byte values in Little Endian byte order.
            - b - One-byte signed integer
            - h - Two-byte signed integer
            - i - Four-byte signed integer

        13.1.3 Floating Point
            F4
            Floating point values are stored using the IEEE754 Single Precision
            4-byte format in Little Endian byte order.
            - f - Four-byte floating-point number (single precision)

        13.1.4 Hex
            H1, H2
            Bit register definitions are stored in unsigned integer format.
            - B - One-byte unsigned integer
            - H - Two-byte unsigned integer

        13.1.5 String
            S5, S21
            String values are stored with length byte first,
            followed by a number of data bytes defined
            with the length byte.
        """

    U1 = 'U1'
    U2 = 'U2'
    U4 = 'U4'

    I1 = 'I1'
    I2 = 'I2'
    I4 = 'I4'

    F4 = 'F4'

    TYPE_S = 'S'

    H1 = 'H1'
    H2 = 'H2'


"""Little-Endian format for the unpack function"""
UNPACK_FORMAT_LITTLE_ENDIAN = '<'

"""Python data unpacking formats"""
UNPACK_FORMATS = {
    DeviceDataFormat.U1: 'B',
    DeviceDataFormat.U2: 'H',
    DeviceDataFormat.U4: 'I',

    DeviceDataFormat.I1: 'b',
    DeviceDataFormat.I2: 'h',
    DeviceDataFormat.I4: 'i',

    DeviceDataFormat.F4: 'f',

    DeviceDataFormat.H1: 'B',
    DeviceDataFormat.H2: 'H',

    DeviceDataFormat.TYPE_S: 's'
}


def load_dump_data(file_name):
    """
    Load data from a dump text file in the format:

    ```
    0xAAAA: [ AA BB .... ]
    0xAAAB: [ CC DD .... ]
    ...
    ```

    :param file_name: Name of the Dump file
    :return: A dictionary where keys are addresses and values are the corresponding values
    """
    retval = {}
    with open(file_name, 'r') as file:
        line = file.readline()
        while line:
            line = line.rstrip()  # remove trailing whitespaces

            addr_str, values_str = line.split(': ')
            addr = int(addr_str, HEX_FORMAT)

            values = values_str.strip('[]').split()
            for value in values:
                retval[addr] = int(value, HEX_FORMAT)
                addr += 1

            line = file.readline()

    return retval


class DataFlashElement:
    def __init__(self, data_format, data_description):
        self.data_format = data_format
        self.data_description = data_description

    def __str__(self) -> str:
        return f'{{type: {self.data_format}; name: {self.data_description}}}'


def load_data_descriptions():
    """14.1 Data Flash Table"""
    with open(DATA_DESCRIPTIONS_FILE, newline='') as csvfile:
        csvreader = csv.reader(csvfile, delimiter=';')
        next(csvreader)  # skip headers
        retval = {}
        for row in csvreader:
            addr = int(row[0], 16)
            data_format = row[1]
            data_description = row[2]
            retval[addr] = DataFlashElement(data_format, data_description)
        return retval


def parse_record(addr, data_description, dataset):
    """Return data record at the address with the description"""
    buf = []
    size = int(data_description.data_format[1:])
    for i in range(0, size):
        buf.append(dataset[addr + i])

    data_format_type = data_description.data_format[0]
    if DeviceDataFormat.TYPE_S == data_format_type:
        _str_len = buf[0]
        bytes_buf = bytes(buf[1:_str_len + 1])
        unpack_format = f'{UNPACK_FORMAT_LITTLE_ENDIAN}{_str_len}{UNPACK_FORMATS[DeviceDataFormat.TYPE_S]}'
    else:
        bytes_buf = bytes(buf)
        unpack_format = f'{UNPACK_FORMAT_LITTLE_ENDIAN}{UNPACK_FORMATS[data_description.data_format]}'

    value = unpack(unpack_format, bytes_buf)[0]
    if DeviceDataFormat.H1 == data_description.data_format:
        value_str = f'0x{value:02X} = 0b{value:08b}'
    elif DeviceDataFormat.H2 == data_description.data_format:
        value_str = f'0x{value:04X} = 0b{value:016b}'
    elif DeviceDataFormat.TYPE_S == data_format_type:
        value_str = value.decode()
    else:
        value_str = str(value)

    return f'0x{addr:04X}: ({data_description.data_format}) [{data_description.data_description}] = {value_str}'


def compare(addr_range, data_descriptions, dataset_1, dataset_2):
    """Compare two datasets and print different records"""
    for addr in addr_range:

        data_description = data_descriptions.get(addr)
        if data_description is None:
            continue

        value_1 = parse_record(addr, data_description, dataset_1)
        value_2 = parse_record(addr, data_description, dataset_2)

        if value_1 != value_2:
            print(f'dataset 1: {value_1}')
            print(f'dataset 2: {value_2}')
            print()


def print_data_detailed(addr_range, data_descriptions, dataset):
    """Print all data with their descriptions, if available"""
    for addr in addr_range:
        data_description = data_descriptions.get(addr)
        if data_description is not None:
            print(parse_record(addr, data_description, dataset))


if __name__ == '__main__':
    _data_descriptions = load_data_descriptions()

    # Load dumps as datasets
    _dump_original = load_dump_data('data/dump/dump-original.txt')
    _dump_20240710 = load_dump_data('data/dump/dump-20240710.txt')
    _dump_20240710_lifetime_reset = load_dump_data('data/dump/dump-20240710-after-lifetime-reset.txt')

    # See the detailed contents of the dumps
    # print_data_detailed(FULL_ADDRESSES_RANGE, _data_descriptions, _dump_original)
    # print_data_detailed(FULL_ADDRESSES_RANGE, _data_descriptions, _dump_20240710)
    # print_data_detailed(FULL_ADDRESSES_RANGE, _data_descriptions, _dump_20240710_lifetime_reset)

    # Compare two dump datasets
    compare(ADDRESSES_WITHOUT_RA_TABLE, _data_descriptions, _dump_original, _dump_20240710)
    # compare(FULL_ADDRESSES_RANGE, _data_descriptions, _dump_20240710, _dump_20240710_lifetime_reset)
