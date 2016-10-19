cmake_minimum_required(VERSION 2.8.8)

# Toolchain configuration, must be before project()
set(SIMPLICITY_STUDIO_ROOT "$ENV{HOME}/opt/SimplicityStudio_v4" CACHE
	STRING "Path to Simplicity Studio")
include(CMakeLists.gecko_toolchain.txt)

project(uart_example C)

# Build configuration
set(TARGET_BOARD "BRD4300AREVA02" CACHE STRING "Development board to build for")
set_property(CACHE TARGET_BOARD PROPERTY STRINGS "BRD4300AREVA01" "BRD4300AREVA02" "BRD4300AREVA03" "BRD4301A")


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wno-main")

# Generate config.h
configure_file(config.h.in "${PROJECT_BINARY_DIR}/config.h")
include_directories("${PROJECT_BINARY_DIR}")

# Build Silabs em* libraries
include(CMakeLists.build_emlib.txt)
include(CMakeLists.build_emdrv.txt)

# Build sub directories
add_subdirectory(gatt_db)
include_directories("${CMAKE_BINARY_DIR}/gatt_db")
add_subdirectory(src)
