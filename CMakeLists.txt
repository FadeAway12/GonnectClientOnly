cmake_minimum_required(VERSION 3.21)

# Project setup
project(Project1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Point CMake to vcpkg (only if not already set by command line)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
      "C:/Users/Vikra/Downloads/VCPKG/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "Vcpkg toolchain file")
endif()

# Dependencies
find_package(Crow CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)

# Source files
set(SOURCES
    Main.cpp
    Board.cpp
    Evaluation.cpp
    Search.cpp
)

# Executable
add_executable(Project1 ${SOURCES})

# Link libraries (Crow + Asio + Windows networking)
target_link_libraries(Project1 PRIVATE Crow::Crow asio::asio ws2_32)
