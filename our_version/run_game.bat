@echo off
echo ========================================================
echo ZombieBirds - Auto Build ^& Run Script (MSYS2 / UCRT64)
echo ========================================================

:: Check if MSYS2 is installed in the default location
if not exist "C:\msys64\usr\bin\bash.exe" (
    echo [ERROR] Could not find MSYS2 at C:\msys64
    echo Please ensure MSYS2 is installed.
    pause
    exit /b 1
)

echo [1/3] Installing required MSYS2 packages (C++, CMake, Make, SFML)...
C:\msys64\usr\bin\bash.exe -lc "pacman -S --needed --noconfirm mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-sfml"

echo.
echo [2/3] Building the game...
:: Temporarily add UCRT64 bin to PATH so CMake and Make can be found
set PATH=C:\msys64\ucrt64\bin;%PATH%

if not exist build mkdir build
cd build

echo Running CMake...
cmake -G "MinGW Makefiles" ..
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b 1
)

echo Running Make...
mingw32-make -j4
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Compilation failed!
    pause
    exit /b 1
)

echo.
echo [3/3] Build Successful! Launching ZombieBirds...
echo ========================================================
cd ..
start build\ZombieBirds.exe
