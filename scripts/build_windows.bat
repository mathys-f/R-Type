@echo off
REM Standalone Windows build script - configures and builds the project using CMake with MinGW-w64
REM Prerequisites: MinGW-w64 (W64Devkit) must be installed and in PATH

setlocal EnableDelayedExpansion

REM Color setup for Windows
set "BLUE=[1;34m"
set "GREEN=[1;32m"
set "YELLOW=[1;33m"
set "RED=[1;31m"
set "RESET=[0m"

echo %BLUE%Starting Windows build script%RESET%

REM Check if MinGW-w64 is available
where gcc >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo %RED%Error: MinGW-w64 not found in PATH.%RESET%
    echo %RED%Please install W64Devkit or MinGW-w64 and add it to your PATH.%RESET%
    echo %YELLOW%Download W64Devkit from: https://github.com/skeeto/w64devkit/releases%RESET%
    pause
    exit /b 1
)

REM Check if CMake is available
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo %RED%Error: CMake not found in PATH.%RESET%
    echo %RED%Please install CMake and add it to your PATH.%RESET%
    pause
    exit /b 1
)

REM Display versions
for /f "tokens=*" %%i in ('gcc --version ^| findstr /r "^gcc"') do echo %GREEN%%i%RESET%
for /f "tokens=*" %%i in ('cmake --version ^| findstr /r "^cmake"') do echo %GREEN%%i%RESET%

REM Create build directory if it doesn't exist
echo %BLUE%Setting up build directory...%RESET%
if not exist "build" mkdir build
cd build
echo %GREEN%Build directory ready.%RESET%

REM Prompt user for build type
set /p build_type="%YELLOW%Build type (Release/Debug) [Debug]: %RESET%"
if "%build_type%"=="" set build_type=Debug

REM Validate build type
if /I not "%build_type%"=="Release" if /I not "%build_type%"=="Debug" (
    echo %RED%Invalid build type. Choose 'Release' or 'Debug'.%RESET%
    pause
    exit /b 1
)

REM Run CMake to configure and build the project
echo %BLUE%Configuring project...%RESET%
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=%build_type% -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
if %ERRORLEVEL% NEQ 0 (
    echo %RED%CMake configuration failed.%RESET%
    pause
    exit /b 1
)
echo %BLUE%Building project...%RESET%
cmake --build .
if %ERRORLEVEL% NEQ 0 (
    echo %RED%Build failed.%RESET%
    pause
    exit /b 1
)
echo %GREEN%Build succeeded%RESET%

REM Copy binaries to parent directory
echo %BLUE%Copying executables...%RESET%
if exist "bin\%build_type%\r-type_client.exe" (
    copy /Y "bin\%build_type%\r-type_client.exe" "..\r-type_client.exe" >nul
)
if exist "bin\%build_type%\r-type_server.exe" (
    copy /Y "bin\%build_type%\r-type_server.exe" "..\r-type_server.exe" >nul
)
echo %GREEN%Executables copied to parent directory.%RESET%

REM Ask user if they want to run the program
echo %YELLOW%Do you want to run the program now ? (y/N)%RESET%
set /p answer=

if /I "%answer%"=="y" (
    cd ..
    echo %BLUE%Launching programs...%RESET%
    
    REM Launch server in new window
    start "R-Type Server" cmd /k "echo Running %build_type% r-type_server && r-type_server.exe -p 8080"
    
    REM Wait a moment for server to start
    timeout /t 2 /nobreak >nul
    
    REM Launch client in new window
    start "R-Type Client" cmd /k "echo Running %build_type% r-type_client && r-type_client.exe"
    
    echo %GREEN%Programs launched in separate windows.%RESET%
) else (
    cd ..
    echo %BLUE%Build complete. You can run the programs manually:^
    
        - r-type_server.exe -p 8080^
        - r-type_client.exe%RESET%
)

pause
