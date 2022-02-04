## --------------------------------------------------------------------------
## Read BME688 sensor values with a Raspberry Pi Pico using the official Bosch-API
##
## Read sensor in forced-mode scanning the parameter space (heater/duration)
##
## Author: Bernhard Bablok
##
## https://github.com/bablokb/pico-bme688-scan
## --------------------------------------------------------------------------

# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
#set(PICO_SDK_PATH "/data/pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-bme688-scan C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(pico-bme688-scan pico-bme688-scan.c )

# program configuration
add_compile_definitions(ALTITUDE_AT_LOC=520)
add_compile_definitions(HEATER_TEMP=100,200,300)
add_compile_definitions(HEATER_DURATION=100,150,300)
add_compile_definitions(UPDATE_INTERVAL=10)

# configuration for the library (override if necessary)
add_compile_definitions(BME68X_DO_NOT_USE_FPU)
add_compile_definitions(BME68X_PORT=i2c1)
add_compile_definitions(BME68X_ADDR=BME68X_I2C_ADDR_HIGH)
add_compile_definitions(BME68X_SDA=2)
add_compile_definitions(BME68X_SCL=3)
#add_compile_definitions(BME68X_I2C_PU)      # use builtin-pullups

# add preprocessor-constant DEBUG for Debug-builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_VERBOSE_MAKEFILE 1)
  add_compile_definitions(DEBUG)
else()
endif()

pico_set_program_name(pico-bme688-scan "pico-bme688-scan")
pico_set_program_version(pico-bme688-scan "0.1")

pico_enable_stdio_uart(pico-bme688-scan 1)
pico_enable_stdio_usb(pico-bme688-scan 1)

# build the bme688-library from our submodule
add_subdirectory(pico-bme688)

# Add any user requested libraries
target_link_libraries(pico-bme688-scan
  pico_stdlib
  hardware_i2c
  bme68x
)

pico_add_extra_outputs(pico-bme688-scan)
