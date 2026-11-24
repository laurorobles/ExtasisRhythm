@echo off
setlocal
echo ============================================================
echo  ExtasisRhythm v4.0.0 - Windows Installer (Free ^& Open Source)
echo ============================================================
echo.

set "SCRIPT_DIR=%~dp0"
if exist "%SCRIPT_DIR%..\ExtasisRhythm.vst3" (
    set "SRC_DIR=%SCRIPT_DIR%..\"
) else (
    set "SRC_DIR=%SCRIPT_DIR%"
)

set "VST3_DIR=%COMMONPROGRAMFILES%\VST3"

if exist "%SRC_DIR%ExtasisRhythm.vst3" (
    echo [1/1] Installing VST3 Plugin to "%VST3_DIR%"...
    if not exist "%VST3_DIR%" mkdir "%VST3_DIR%" 2>nul
    xcopy /E /I /Y /Q "%SRC_DIR%ExtasisRhythm.vst3" "%VST3_DIR%\ExtasisRhythm.vst3"
    if errorlevel 1 (
        echo.
        echo [!] Could not write to "%VST3_DIR%".
        echo     Please right-click INSTALL_WINDOWS.bat and select "Run as administrator".
        pause
        exit /b 1
    )
) else (
    echo [!] ExtasisRhythm.vst3 not found in "%SRC_DIR%".
    pause
    exit /b 1
)

echo.
echo ============================================================
echo  Installation Complete! Open your DAW and rescan plugins.
echo  Tip: You can also run ExtasisRhythm.exe directly (Standalone).
echo  Licensed under GPL-3.0 ^| Extasis Records (@laurorobles)
echo ============================================================
pause
