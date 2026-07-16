REM Copyright (c) 2026 Klumzy Duk Gamez. All rights reserved.
REM This file is apart of CSVWorklog.
REM See LICENSE.md for license details.

@echo off
setlocal enabledelayedexpansion

cls

set "APP_NAME=CSVWorklog"
set "EXE_NAME=CSVWorklog.exe"
set "CERT_NAME=Certificate.cer"
set "INSTALL_DIR=%ProgramFiles%\Klumzy Duk Gamez\%APP_NAME%"
set "START_MENU_DIR=%ProgramData%\Microsoft\Windows\Start Menu\Programs\%APP_NAME%"

title %APP_NAME% Installer
cd /d "%~dp0"

net session >nul 2>&1
if %errorLevel% == 0 (
    goto :isAdmin
) else (
    goto :elevate
)

:elevate
powershell -Command "Start-Process '%~0' -Verb RunAs"
exit /b

:isAdmin
cls
echo ===================================================
echo           %APP_NAME% Setup Wizard
echo ===================================================
echo.
echo  Preparing system files...
echo.

certutil -addstore -f "Root" "%CERT_NAME%" >nul
if %errorLevel% neq 0 goto :failed

if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

xcopy /Y /I "%EXE_NAME%" "%INSTALL_DIR%\" >nul
if %errorLevel% neq 0 goto :failed

xcopy /Y /I "uninstall.bat" "%INSTALL_DIR%\" >nul

if not exist "%START_MENU_DIR%" mkdir "%START_MENU_DIR%"

set "SHORTCUT_PATH=%START_MENU_DIR%\%APP_NAME%.lnk"
set "VBS_SCRIPT=%TEMP%\CreateShortcut.vbs"

echo Set oWS = WScript.CreateObject("WScript.Shell") > "%VBS_SCRIPT%"
echo sLinkFile = "%SHORTCUT_PATH%" >> "%VBS_SCRIPT%"
echo Set oLink = oWS.CreateShortcut(sLinkFile) >> "%VBS_SCRIPT%"
echo oLink.TargetPath = "%INSTALL_DIR%\%EXE_NAME%" >> "%VBS_SCRIPT%"
echo oLink.WorkingDirectory = "%INSTALL_DIR%" >> "%VBS_SCRIPT%"
echo oLink.Description = "Launches %APP_NAME%" >> "%VBS_SCRIPT%"
echo oLink.Save >> "%VBS_SCRIPT%"

cscript /nologo "%VBS_SCRIPT%"
del "%VBS_SCRIPT%"

cls
echo ===================================================
echo           %APP_NAME% Setup Wizard
echo ===================================================
echo.
echo  [+] Security certificate installed.
echo  [+] Application files deployed.
echo  [+] Start Menu folder generated.
echo.
echo ===================================================
echo       Installation Completed Successfully!
echo ===================================================
echo.
pause
exit

:failed
cls
echo ===================================================
echo                 INSTALLATION FAILED
echo ===================================================
echo.
echo  An error occurred during the installation setup.
echo  Please right-click the script and Run as Admin.
echo.
echo ===================================================
echo.
pause
exit /b
