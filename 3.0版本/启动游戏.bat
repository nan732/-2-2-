@echo off
setlocal

cd /d "%~dp0"

where cmake >nul 2>nul
if errorlevel 1 (
    echo CMake was not found. Please install CMake and add it to PATH.
    pause
    exit /b 1
)

cmake -S . -B build
if errorlevel 1 (
    echo Build cache may point to an old folder. Recreating build directory...
    if exist build rmdir /s /q build
    cmake -S . -B build
)
if errorlevel 1 (
    pause
    exit /b 1
)

cmake --build build --config Release
if errorlevel 1 (
    pause
    exit /b 1
)

set "GAME_EXE="
if exist "build\Release\TrapMindBall.exe" set "GAME_EXE=build\Release\TrapMindBall.exe"
if not defined GAME_EXE if exist "build\Debug\TrapMindBall.exe" set "GAME_EXE=build\Debug\TrapMindBall.exe"
if not defined GAME_EXE if exist "build\TrapMindBall.exe" set "GAME_EXE=build\TrapMindBall.exe"
if not defined GAME_EXE for /f "delims=" %%F in ('dir /b /s "build\TrapMindBall*.exe" 2^>nul') do (
    set "GAME_EXE=%%F"
    goto :run_game
)

:run_game
if defined GAME_EXE (
    "%GAME_EXE%"
) else (
    echo Game executable was not found after building.
    pause
    exit /b 1
)

endlocal
