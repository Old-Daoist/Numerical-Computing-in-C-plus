@echo off
setlocal enabledelayedexpansion

echo ===============================
echo NUMERICAL COMPUTING C++ BUILD
echo ===============================

cd /d "D:\SCMS\C++ programs\Numerical Computing Library\NumericalLibrary-ver2"

echo.
echo [1] Collecting source files (excluding tests)...

set FILES=

for /R %%f in (*.cpp) do (
    echo %%f | findstr /I "tests" >nul
    if errorlevel 1 (
        set FILES=!FILES! "%%f"
    )
)

echo Files used:
echo !FILES!

echo.
echo [2] Compiling project...

g++ -std=c++17 !FILES! ^
-I library\MatrixV1\include ^
-o numerical_app.exe

IF %ERRORLEVEL% NEQ 0 (
    echo.
    echo Compilation FAILED!
    pause
    exit /b
)

echo.
echo [3] Running program...
numerical_app.exe

echo.
echo ===============================
echo DONE
echo ===============================
pause