@echo off
echo ===============================
echo MATRIXV1 PYTHON SETUP + TEST
echo ===============================

cd /d "D:\SCMS\C++ programs\Numerical Computing Library\NumericalLibrary-ver2\library\MatrixV1_Python"

echo.
echo [1] Using correct Python...
set PYTHON_PATH=D:\Python\python.exe

echo.
echo [2] Installing package...
%PYTHON_PATH% -m pip install -e .

echo.
echo [3] Installing pytest...
%PYTHON_PATH% -m pip install pytest

echo.
echo [4] Running tests...
%PYTHON_PATH% -m pytest -v

echo.
echo DONE.
pause