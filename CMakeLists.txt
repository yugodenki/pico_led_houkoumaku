cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PROJECT_NAME pico_houkoumaku)

# Pull in SDK
include(src/pico_sdk_import.cmake)
include(src/pico_extras_import_optional.cmake)

project(${PROJECT_NAME} C CXX ASM)

# Initialize SDK
pico_sdk_init()
add_subdirectory(src)

