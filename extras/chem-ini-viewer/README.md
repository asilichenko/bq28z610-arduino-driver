# Chem.ini Viewer

This is custom viewer for Texas Instruments chemistry config file: 'Chem.ini'.

You can find this file in the bqStudio installation folder, for example:

`C:\Program Files (x86)\bqStudio\BatteryManagementStudio\chemistry\`

<img width="600" alt="main window" src="https://github.com/user-attachments/assets/e40ce6b9-14f0-4acf-9efe-933d2b75438f" />


# Build process

1. Create virtual environment
    ```
    python -m venv .venv
    ```

2. Activate virtual environment
    ```
    .venv\Scripts\activate
    ```

3. Upgrade pip and wheel
    ```
    python -m pip install --upgrade pip setuptools wheel
    ```

4. Install requirements
    ```
    pip install -r requirements.txt
    ```

5. Run
   ```
   python -m chem_ini_viewer
   ```

6. Make exe
   ```
   python build.py
   ```

# Sources

- [Chem.ini](chem_ini_viewer/Chem.ini) - original TI file
- [app.py](chem_ini_viewer/app.py) - main script to run
- [build.py](build.py) - script to make exe
- [ChemIniViewer.exe](dist/ChemIniViewer/ChemIniViewer.exe) - executable
