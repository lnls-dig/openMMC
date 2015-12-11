# Define minimal required version of CMake.
cmake_minimum_required(VERSION 2.8.7)

# Project definition
project(afcipm C CXX ASM)

##
# CMake environment settings
#

if(NOT CMAKE_TOOLCHAIN_FILE)
  message(FATAL_ERROR "No toolchain configuration file specified.")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Include user configuration
include(${CMAKE_MODULE_PATH}/config.cmake)


set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/out)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
    "Choose the type of build, options are: none Debug Release."
    FORCE)
endif()

set(CMAKE_ERROR_FLAGS "-Wall -Wextra -Wmissing-prototypes -Wpointer-arith -Wno-packed-bitfield-compat")

set(CMAKE_BUILD_FLAGS "-mcpu=${TARGET_CPU} -mtune=${TARGET_CPU} -march=${TARGET_ARCH}")
set(CMAKE_BUILD_FLAGS "${CMAKE_BUILD_FLAGS} -mthumb -mthumb-interwork -mno-sched-prolog -mapcs-frame")
set(CMAKE_BUILD_FLAGS "${CMAKE_BUILD_FLAGS} -DTARGET_CONTROLLER=${TARGET_CONTROLLER}")

set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} ${CMAKE_BUILD_FLAGS} ${CMAKE_ERROR_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS} -O0 -g3 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS} -O3")

set(CMAKE_EXE_LINKER_FLAGS "-T ${CMAKE_SOURCE_DIR}/linker/lpc1764.ld -Wl,-Map=${CMAKE_SOURCE_DIR}/linker/afcipm.map -Wl,-gc-sections --specs=nosys.specs -nostdlib -nostartfiles -mthumb -march=${TARGET_ARCH}")

if(${TARGET_CPU} MATCHES "^(lpc|LPC)17")
  set(CMAKE_BUILD_FLAGS "${CMAKE_BUILD_FLAGS} -D__CODE_RED -D__USE_LPCOPEN -DNO_BOARD_LIB -D__LPC17XX__ -D__NEWLIB__")
endif()

# When we break up long strings in CMake we get semicolon
# separated lists, undo this here...
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" CACHE STRING "")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "")

# Source paths
set(PROJ_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/afcipm.c)
add_subdirectory(FreeRTOS)
add_subdirectory(port)
add_subdirectory(modules)

#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#  add_subdirectory(trace)
#endif()

# When we break up long strings in CMake we get semicolon
# separated lists, undo this here...
#string(REGEX REPLACE ";" " " PROJ_SRCS "${PROJ_SRCS}")
#set(PROJ_SRCS "${PROJ_SRCS}" CACHE STRING "")

## Create executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_executable(afcipm ${PROJ_SRCS})
target_include_directories(afcipm PUBLIC ${PROJ_HDRS})
