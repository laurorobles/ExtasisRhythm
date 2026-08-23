@echo off
setlocal
echo ============================================================
echo  EXTASIS RHYTHM - Windows Automated Installer
echo ============================================================

set "INSTALL_DIR=%~dp0"

echo [1/2] Installing Sample Library to %USERPROFILE%\Documents\ExtasisRhythm_Samples...
if not exist "%USERPROFILE%\Documents\ExtasisRhythm_Samples" mkdir "%USERPROFILE%\Documents\ExtasisRhythm_Samples"
if exist "%INSTALL_DIR%ExtasisRhythm_Samples" (
    xcopy /E /I /Y /Q "%INSTALL_DIR%ExtasisRhythm_Samples" "%USERPROFILE%\Documents\ExtasisRhythm_Samples"
)

echo [2/2] Installing VST3 Plugin to C:\Program Files\Common Files\VST3...
if not exist "C:\Program Files\Common Files\VST3" mkdir "C:\Program Files\Common Files\VST3"
if exist "%INSTALL_DIR%ExtasisRhythm.vst3" (
    xcopy /E /I /Y /Q "%INSTALL_DIR%ExtasisRhythm.vst3" "C:\Program Files\Common Files\VST3\ExtasisRhythm.vst3"
)

echo.
echo ============================================================
echo  Installation Complete! Open your DAW and rescan plugins.
echo ============================================================
pause
