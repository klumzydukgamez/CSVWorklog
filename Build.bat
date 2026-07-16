REM Copyright (c) 2026 Klumzy Duk Gamez. All rights reserved.
REM This file is apart of CSVWorklog.
REM See LICENSE.md for license details.

@echo off

REM Capture escape character
for /f %%a in ('echo prompt $E^| cmd') do set "ESC=%%a"

cls
echo %ESC%[93mCONFIGURING%ESC%[0m
echo.
cmake -S . -B Build -G Ninja
echo.

echo %ESC%[93mBUILDING%ESC%[0m
echo.
cmake --build Build
echo.

echo %ESC%[93mINSTALLING%ESC%[0m
echo.
cmake --install Build
echo Copying compile_commands.json
copy Build\compile_commands.json compile_commands.json
echo.

echo %ESC%[95mRUNNING%ESC%[0m
echo.
cd Bin
CSVWorkLog.exe
echo.

pause
cd ..
cls

