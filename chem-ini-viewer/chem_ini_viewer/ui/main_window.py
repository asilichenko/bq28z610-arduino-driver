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

from PySide6.QtGui import QAction
from PySide6.QtUiTools import QUiLoader
from PySide6.QtWidgets import QPushButton, QSpinBox, QTableView, QLineEdit, QComboBox, QDialog

from chem_ini_viewer.ui.base_window import BaseWindow


class MainWindow(BaseWindow):
    TITLE: str = 'Chem.ini Viewer'

    def __init__(self, proxy):
        self._proxy = proxy
        super().__init__('main_window.ui')
        self.set_title()

    def _setup_ui(self) -> None:
        self.action_open: QAction = self._find_widget(QAction, 'actionOpen')
        self.action_about: QAction = self._find_widget(QAction, 'actionAbout')

        self.chem_edit: QLineEdit = self._find_widget(QLineEdit, "chem_edit")
        self.search_edit: QLineEdit = self._find_widget(QLineEdit, "search_edit")
        self.cap_min: QSpinBox = self._find_widget(QSpinBox, "cap_min")
        self.cap_max: QSpinBox = self._find_widget(QSpinBox, "cap_max")
        self.desc_combo: QComboBox = self._find_widget(QComboBox, "desc_combo")
        self.btn_reset: QPushButton = self._find_widget(QPushButton, "btn_reset")

        self.table: QTableView = self._find_widget(QTableView, "tableView")

    def reset_filters(self):
        self.chem_edit.clear()
        self.search_edit.clear()
        self.cap_min.setValue(0)
        self.cap_max.setValue(0)
        self.desc_combo.setCurrentIndex(0)

    def set_title(self, version: int | None = None, path: str | None = None) -> None:
        self.setWindowTitle(f'{self.TITLE} [File Version: {version} - {path}]' if path else self.TITLE)

    def show_about(self):
        loader = QUiLoader()
        # noinspection PyTypeChecker
        dialog: QDialog = loader.load(self.UI_DIR / 'about.ui', self)
        dialog.exec()
