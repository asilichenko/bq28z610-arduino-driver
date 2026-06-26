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

from pathlib import Path

from PySide6.QtCore import QFile
from PySide6.QtUiTools import QUiLoader
from PySide6.QtWidgets import QMainWindow, QWidget

ROOT: Path = Path(__file__).parent.parent


class BaseWindow(QMainWindow):
    UI_DIR: Path = ROOT / 'ui'

    def __init__(self, ui_path: str | Path, parent: QWidget | None = None) -> None:
        super().__init__(parent)

        self._ui_path = self.UI_DIR / ui_path
        # noinspection PyTypeChecker
        self._ui: QMainWindow = self._load_ui(self._ui_path)

        self.setCentralWidget(self._ui)
        self.setWindowTitle(self._ui.windowTitle())
        self.resize(self._ui.size())

        self.statusbar = self._ui.statusBar()

        self._setup_ui()
        self._connect_signals()

    # ------------------------------------------------------------------
    # Private helpers
    # ------------------------------------------------------------------

    @staticmethod
    def _load_ui(path: Path) -> QWidget:
        if not path.exists():
            raise FileNotFoundError(f"UI file not found: {path}")

        ui_file = QFile(str(path))
        if not ui_file.open(QFile.OpenModeFlag.ReadOnly):
            raise RuntimeError(f"Cannot open UI file: {path}")

        loader = QUiLoader()
        widget = loader.load(ui_file)
        ui_file.close()

        if widget is None:
            raise RuntimeError(f"QUiLoader failed to load: {path}\n{loader.errorString()}")

        return widget

    def _find_widget[T](self, widget_type: type[T], name: str) -> T:
        widget = self._ui.findChild(widget_type, name)
        if widget is None:
            raise RuntimeError(f"Widget '{name}' not found in UI")
        return widget

    # ------------------------------------------------------------------
    # Extension points for subclasses
    # ------------------------------------------------------------------

    def _setup_ui(self) -> None:
        pass

    def _connect_signals(self) -> None:
        pass
