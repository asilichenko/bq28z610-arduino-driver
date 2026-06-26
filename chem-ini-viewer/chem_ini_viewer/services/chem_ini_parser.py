#  MIT License
#
#  Copyright (c) 2026 Oleksii Sylichenko
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.

import configparser
import re

from chem_ini_viewer.model import Chem, Cell, ChemIni, ChemOption, Section, FileOption, ChemistryOption

CAP_RE = re.compile(r'\((\d+)\s*(mAh|Ah|A)\)', re.IGNORECASE)


def parse_ini(path: str) -> ChemIni:
    config = configparser.RawConfigParser()
    config.optionxform = str  # use original case
    config.read(path, encoding='cp1251')

    chem_ini = ChemIni(config.getint(Section.File, FileOption.Version))

    num_chem_files: int = config.getint(Section.Chemistry, ChemistryOption.NumChemFiles)
    for i_chem in range(1, num_chem_files + 1):

        section = config[f'{i_chem}_Chem']
        chem = Chem(
            chem_id=section.get(ChemOption.ChemID, ''),
            description=section.get(ChemOption.Description, '')
        )

        num_known_cells: int = int(section.get(ChemOption.NumKnownCells, 0))
        for i_cell in range(1, num_known_cells + 1):
            model = section.get(str(i_cell), '')
            chem.cells.append(_parse_cell_line(model))

        chem_ini.chemistry.append(chem)

    return chem_ini


def _parse_cell_line(value: str) -> Cell:
    if ':' in value:
        mfr, rest = value.split(':', 1)
        mfr = mfr.strip()
        rest = rest.strip()
    else:
        mfr = value.strip()
        rest = ''

    cap_mah = None
    # try capacity from rest first, then from mfr (no-colon case)
    src = rest if rest else mfr
    m = CAP_RE.search(src)
    if m:
        cap_mah = _to_mah(int(m.group(1)), m.group(2))
        if rest:
            model = rest[:m.start()].strip().rstrip(';').strip()
        else:
            mfr = src[:m.start()].strip()
            model = ''
    else:
        model = rest.strip()

    return Cell(mfr, model, cap_mah)


def _to_mah(value: int, unit: str) -> int:
    return value * 1000 if unit.lower() in ('ah', 'a') else value


if __name__ == '__main__':
    print(parse_ini('../Chem.ini'))
