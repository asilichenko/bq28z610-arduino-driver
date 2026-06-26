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

from PySide6.QtCore import QObject, Qt
from PySide6.QtGui import QStandardItemModel, QStandardItem
from PySide6.QtWidgets import QFileDialog

from chem_ini_viewer.model import MultiFilterProxyModel, ChemIni
from chem_ini_viewer.services import chem_ini_parser
from chem_ini_viewer.ui import MainWindow


def _numeric_item(value: int | None) -> QStandardItem:
    item = QStandardItem(str(value or ''))
    item.setData(value if value else -1, Qt.ItemDataRole.UserRole)
    item.setTextAlignment(Qt.AlignmentFlag.AlignRight | Qt.AlignmentFlag.AlignVCenter)
    return item


class MainWindowController(QObject):

    def __init__(self, window: MainWindow, proxy: MultiFilterProxyModel, model: QStandardItemModel) -> None:
        super().__init__()
        self._wnd = window
        self._model: QStandardItemModel = model
        self._proxy = proxy
        self._chem_ini: ChemIni | None = None

        self._setup_table()

        proxy.rowsInserted.connect(self.update_status)
        proxy.rowsRemoved.connect(self.update_status)
        proxy.layoutChanged.connect(self.update_status)

        self._wnd.action_open.triggered.connect(self._open_file)
        self._wnd.action_about.triggered.connect(self._wnd.show_about)
        self._setup_signals()

    def _setup_table(self):
        self._wnd.table.setModel(self._proxy)
        self._wnd.table.setColumnWidth(0, 90)
        self._wnd.table.setColumnWidth(1, 246)
        self._wnd.table.setColumnWidth(2, 105)

    def _setup_signals(self):
        self._wnd.chem_edit.textChanged.connect(lambda t: self._proxy.set_chem_id(t))
        self._wnd.search_edit.textChanged.connect(lambda t: self._proxy.set_mfr_model(t))
        self._wnd.cap_min.valueChanged.connect(lambda v: self._proxy.set_cap_min(v))
        self._wnd.cap_max.valueChanged.connect(lambda v: self._proxy.set_cap_max(v))
        self._wnd.desc_combo.currentTextChanged.connect(lambda t: self._proxy.set_description(t))
        self._wnd.btn_reset.clicked.connect(self._wnd.reset_filters)

    def show_window(self):
        self._wnd.show()

    def _open_file(self):
        path, _ = QFileDialog.getOpenFileName(self._wnd, 'Open Chem.ini', '', 'INI files (*.ini);;All files (*)')
        if path:
            self.load_file(path)

    def load_file(self, path: str) -> None:
        chem_ini: ChemIni = chem_ini_parser.parse_ini(path)
        self._chem_ini = chem_ini
        self._wnd.set_title(chem_ini.version, path)
        self._populate_model()

    def update_status(self) -> None:
        shown = self._proxy.rowCount()
        total = self._model.rowCount()
        self._wnd.statusbar.showMessage(f'{shown} / {total} rows')

    def _populate_model(self):
        if self._chem_ini is None:
            return

        descriptions = sorted({chem.description for chem in self._chem_ini.chemistry})

        self._model.removeRows(0, self._model.rowCount())

        self._wnd.desc_combo.blockSignals(True)
        self._wnd.desc_combo.clear()
        self._wnd.desc_combo.addItem('All')
        self._wnd.desc_combo.addItems(descriptions)
        self._wnd.desc_combo.blockSignals(False)

        for chem in self._chem_ini.chemistry:
            for cell in chem.cells:
                id_item = QStandardItem(chem.chem_id)
                id_item.setTextAlignment(Qt.AlignmentFlag.AlignCenter)

                items = [
                    id_item,
                    QStandardItem(chem.description),
                    _numeric_item(cell.capacity),
                    QStandardItem(cell.manufacturer),
                    QStandardItem(cell.model or ''),
                ]
                for it in items:
                    it.setEditable(False)
                self._model.appendRow(items)

        self.update_status()
