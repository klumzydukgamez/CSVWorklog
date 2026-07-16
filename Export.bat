REM Copyright (c) 2026 Klumzy Duk Gamez. All rights reserved.
REM This file is apart of CSVWorklog.
REM See LICENSE.md for license details.

@echo off
setlocal enabledelayedexpansion

cls

set "APP_NAME=CSVWorklog"
set "EXE_NAME=CSVWorklog.exe"
set "CERT_NAME=Certificate.cer"
set "ZIP_NAME=%APP_NAME%.zip"
set "STAGE_DIR=Export"

title %APP_NAME% Packaging System
cd /d "%~dp0"

echo ===================================================
echo           %APP_NAME% Export Pipeline
echo ===================================================
echo.
echo  [1/3] Validating and structuring target files...

if not exist "Bin\%EXE_NAME%" (
    echo [ERROR] %EXE_NAME% is missing from the Bin directory.
    goto :failed
)
if not exist "%CERT_NAME%" (
    echo [ERROR] %CERT_NAME% is missing from the root directory.
    goto :failed
)
if not exist "install.bat" (
    echo [ERROR] install.bat is missing from the root directory.
    goto :failed
)
if not exist "uninstall.bat" (
    echo [ERROR] uninstall.bat is missing from the root directory.
    goto :failed
)

if exist "%STAGE_DIR%" rmdir /S /Q "%STAGE_DIR%"
mkdir "%STAGE_DIR%"

copy /Y "install.bat" "%STAGE_DIR%\" >nul
copy /Y "uninstall.bat" "%STAGE_DIR%\" >nul
copy /Y "%CERT_NAME%" "%STAGE_DIR%\" >nul
copy /Y "Bin\%EXE_NAME%" "%STAGE_DIR%\" >nul

echo  [2/3] Compressing deployment package into ZIP...

if exist "%ZIP_NAME%" del "%ZIP_NAME%"
powershell -Command "Compress-Archive -Path '%STAGE_DIR%\*' -DestinationPath '%ZIP_NAME%' -Force"

if %errorLevel% neq 0 (
    echo [ERROR] Failed to compile target directory into ZIP package.
    goto :failed
)

echo  [3/3] Finalising file structure cleanup...
rmdir /S /Q "%STAGE_DIR%"

cls
echo ===================================================
echo           %APP_NAME% Export Pipeline
echo ===================================================
echo.
echo  [+] Binary and security files mapped.
echo  [+] ZIP file generated successfully.
echo.
echo ===================================================
echo       Distribution Package Ready For Delivery!
echo ===================================================
echo.
pause
exit

:failed
echo.
echo ===================================================
echo                 EXPORT PIPELINE FAILED
echo ===================================================
echo.
if exist "%STAGE_DIR%" rmdir /S /Q "%STAGE_DIR%"
pause
exit /b
