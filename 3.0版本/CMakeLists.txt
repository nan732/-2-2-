cmake_minimum_required(VERSION 3.16)

project(TrapMindBall LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(raylib)

add_executable(TrapMindBall
    main.cpp
    src/Game.cpp
    src/Level.cpp
    src/MathUtil.cpp
    src/PlayerBall.cpp
    src/Trap.cpp
    src/UI.cpp
)

target_include_directories(TrapMindBall PRIVATE include)
target_link_libraries(TrapMindBall PRIVATE raylib)

if(MSVC)
    target_compile_options(TrapMindBall PRIVATE /W4)
else()
    target_compile_options(TrapMindBall PRIVATE -Wall -Wextra -pedantic)
endif()
