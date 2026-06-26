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

import sys
from pathlib import Path

from PySide6.QtGui import QStandardItemModel
from PySide6.QtWidgets import QApplication
from chem_ini_viewer.controllers import MainWindowController
from chem_ini_viewer.ui import MainWindow
from chem_ini_viewer.model import MultiFilterProxyModel, HEADERS


def _root() -> Path:
    if getattr(sys, 'frozen', False):
        return Path(sys.executable).parent
    return Path(__file__).parent


def run():
    app = QApplication(sys.argv)

    model = QStandardItemModel(0, len(HEADERS))
    model.setHorizontalHeaderLabels(HEADERS.values())

    proxy = MultiFilterProxyModel()
    proxy.setSourceModel(model)

    win = MainWindow(proxy)
    con = MainWindowController(win, proxy, model)

    default_chem_ini: Path = _root() / 'Chem.ini'
    if default_chem_ini.exists():
        con.load_file(str(default_chem_ini))

    con.show_window()
    sys.exit(app.exec())


if __name__ == '__main__':
    run()
