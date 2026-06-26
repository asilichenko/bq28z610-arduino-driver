# Chem.ini Viewer

This is custom viewer for Texas Instruments chemistry config file: 'Chem.ini'.

You can find this file in the bqStudio installation folder, for example:

`C:\Program Files (x86)\bqStudio\BatteryManagementStudio\chemistry\`

# Build process

1. Create virtual environment
    ```
    python -m venv .venv
    ```

2. Activate virtual environment
    ```
    .venv\Scripts\activate.bat
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
- Chem.ini - original TI file
- app.py - main script to run
- build.py - script to make exe