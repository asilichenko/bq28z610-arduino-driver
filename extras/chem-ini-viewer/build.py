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

import os
import shutil
import subprocess

PROJECT_NAME: str = 'ChemIniViewer'
DIST_ROOT: str = 'dist'
DIST_PATH: str = f'{DIST_ROOT}/{PROJECT_NAME}/'

RESOURCES: list[str] = ['Chem.ini']


def clean() -> None:
    print('Clean')

    if os.path.exists(DIST_PATH):
        print('\tRemove dist directory: %s', DIST_PATH)
        shutil.rmtree(DIST_PATH)


def make_exe() -> None:
    print('Make exe file')
    subprocess.run([
        'pyinstaller',
        '--clean',
        '--distpath=' + DIST_PATH,
        'main.spec',
    ])


def copy_resources() -> None:
    print('Copy resources')
    for resource in RESOURCES:
        print(f'\tCopy resource: {resource}')

        src: str = f'chem_ini_viewer/{resource}'
        dst: str = f'{DIST_PATH}{resource}'
        if os.path.isdir(resource):
            shutil.copytree(src, dst)
        else:
            shutil.copyfile(src, dst)


def build():
    print('Start building')
    clean()
    make_exe()
    copy_resources()
    print('Finish building')


if __name__ == '__main__':
    build()
