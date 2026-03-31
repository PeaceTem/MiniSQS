cmake_minimum_required(VERSION 3.10)

# Project name
project(MiniSQS)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include src folder headers
include_directories(src)

# Find all .cpp files recursively in src/
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable from all sources
add_executable(MiniSQS ${SOURCES})