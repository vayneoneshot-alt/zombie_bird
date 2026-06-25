cmake_minimum_required(VERSION 3.10)
project(ZombieBirds)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Include directories
include_directories(src)

# Find all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
list(APPEND SOURCES "src/core/PhysicsWorld.cpp")

# Set output directory to source directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Create executable
add_executable(ZombieBirds ${SOURCES})

# Link SFML libraries
target_link_libraries(ZombieBirds PRIVATE sfml-graphics sfml-window sfml-system)
