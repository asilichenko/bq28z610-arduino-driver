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

from enum import IntEnum

from PySide6.QtCore import Qt, QSortFilterProxyModel


class Column(IntEnum):
    CHEM_ID = 0
    DESCRIPTION = 1
    CAPACITY = 2
    MANUFACTURER = 3
    MODEL = 4


HEADERS = {
    Column.CHEM_ID: "Chemistry ID",
    Column.DESCRIPTION: "Description",
    Column.CAPACITY: "Capacity (mAh)",
    Column.MANUFACTURER: "Manufacturer",
    Column.MODEL: "Model",
}


class MultiFilterProxyModel(QSortFilterProxyModel):
    def __init__(self, parent=None):
        super().__init__(parent)
        self._chem_id = ''
        self._desc = ''
        self._cap_min = None
        self._cap_max = None
        self._mfr_model = ''

    def set_mfr_model(self, text: str):
        self._mfr_model = text.lower()
        self.invalidate()

    def set_description(self, text: str):
        self._desc = text if text != 'All' else ''
        self.invalidate()

    def set_cap_min(self, val: int):
        self._cap_min = val if val > 0 else None
        self.invalidate()

    def set_cap_max(self, val: int):
        self._cap_max = val if val > 0 else None
        self.invalidate()

    def set_chem_id(self, text: str):
        self._chem_id = text.strip()
        self.invalidate()

    def lessThan(self, left, right):
        if left.column() == Column.CAPACITY:
            lv = left.data(Qt.ItemDataRole.UserRole)
            rv = right.data(Qt.ItemDataRole.UserRole)
            return (lv or -1) < (rv or -1)
        return super().lessThan(left, right)

    def filterAcceptsRow(self, source_row: int, source_parent) -> bool:
        model = self.sourceModel()

        def cell(col: Column):
            idx = model.index(source_row, col.value, source_parent)
            return model.data(idx) or ''

        if self._chem_id:
            if self._chem_id.lower() not in cell(Column.CHEM_ID).lower():
                return False

        if self._mfr_model:
            combined = (cell(Column.MANUFACTURER) + ' ' + cell(Column.MODEL)).lower()
            if self._mfr_model not in combined:
                return False

        if self._desc:
            if cell(Column.DESCRIPTION) != self._desc:
                return False

        cap_raw = cell(Column.CAPACITY)
        cap_val = int(cap_raw) if cap_raw else None

        if self._cap_min is not None:
            if cap_val is None or cap_val < self._cap_min:
                return False
        if self._cap_max is not None:
            if cap_val is None or cap_val > self._cap_max:
                return False

        return True
