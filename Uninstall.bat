REM Copyright (c) 2026 Klumzy Duk Gamez. All rights reserved.
REM This file is apart of CSVWorklog.
REM See LICENSE.md for license details.

@echo off
setlocal enabledelayedexpansion

cls

set "APP_NAME=CSVWorklog"
set "CERT_NAME=Certificate.cer"
set "INSTALL_DIR=%ProgramFiles%\Klumzy Duk Gamez\%APP_NAME%"
set "START_MENU_DIR=%ProgramData%\Microsoft\Windows\Start Menu\Programs\%APP_NAME%"

title %APP_NAME% Uninstaller
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
echo           %APP_NAME% Uninstaller
echo ===================================================
echo.
echo  Are you sure you want to completely remove %APP_NAME%?
echo  Press any key to continue or close this window to cancel.
echo.
echo ===================================================
echo.
pause >nul

cls
echo ===================================================
echo           %APP_NAME% Uninstaller
echo ===================================================
echo.
echo  Removing system files...
echo.

if exist "%START_MENU_DIR%" rmdir /S /Q "%START_MENU_DIR%"

if exist "%INSTALL_DIR%\%CERT_NAME%" (
    certutil -delstore "Root" "%CERT_NAME%" >nul
)

cls
echo ===================================================
echo           %APP_NAME% Uninstaller
echo ===================================================
echo.
echo  [-] Start Menu folder removed.
echo  [-] Security certificate uninstalled.
echo.
echo  Finishing file cleanup...
echo.
echo ===================================================
echo       Uninstallation Completed Successfully!
echo ===================================================
echo.

(goto) 2>nul & rmdir /S /Q "%INSTALL_DIR%" & exit
