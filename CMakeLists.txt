# Define minimal required version of CMake.
cmake_minimum_required(VERSION 2.8.7)

# Project definition
project(afcipm C CXX ASM)

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

##
# CMake environment settings
#
if(NOT CMAKE_TOOLCHAIN_FILE)
  message(WARNING "${ColourBold}${Red}No toolchain configuration file specified. Using default option!${ColourReset}")

  include(build_cfg/toolchain/toolchain-arm-none-eabi.cmake)

endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build_cfg")

# Include user configuration
include(${CMAKE_MODULE_PATH}/config.cmake)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
    "Choose the type of build, options are: none Debug Release."
    FORCE)
endif()

set(CMAKE_ERROR_FLAGS "-Wall -Wextra -Wpointer-arith -Wno-packed-bitfield-compat -Wno-unused-parameter -Wno-missing-field-initializers")

set(CMAKE_BUILD_FLAGS "-mcpu=${TARGET_CPU} -mtune=${TARGET_CPU} -march=${TARGET_ARCH}")
set(CMAKE_BUILD_FLAGS "${CMAKE_BUILD_FLAGS} -mthumb -mthumb-interwork -mno-sched-prolog -mapcs-frame")
set(CMAKE_BUILD_FLAGS "${CMAKE_BUILD_FLAGS} -DTARGET_CONTROLLER=${TARGET_CONTROLLER}")

set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} ${CMAKE_BUILD_FLAGS} ${CMAKE_ERROR_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG     "-O0 -g3 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE   "-O3")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_SOURCE_DIR}/linker/lpc1764.ld -Wl,-Map=${CMAKE_SOURCE_DIR}/linker/afcipm.map")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

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
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_subdirectory(trace)
endif()

#Link libraries
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

## Create executable
add_executable(${CMAKE_PROJECT_NAME}.axf ${PROJ_SRCS})
target_include_directories(${CMAKE_PROJECT_NAME}.axf PUBLIC ${PROJ_HDRS})
target_link_libraries(${CMAKE_PROJECT_NAME}.axf gcc c m)
target_link_libraries(${CMAKE_PROJECT_NAME}.axf FreeRTOS)
target_link_libraries(${CMAKE_PROJECT_NAME}.axf lpcopen)

##Program command
set(LPCXPRESSO_PATH /usr/local/lpcxpresso_7.8.0_426/lpcxpresso/)

find_program(DFU_UTIL dfu-util)
find_program(NXP_PROG crt_emu_cm3_nxp PATHS ${LPCXPRESSO_PATH}/bin NO_DEFAULT_PATH)

if(NOT DFU_UTIL)
  message(WARNING "${ColourBold}${Red}Dfu-util not found! Can't boot the LPCLink board!${ColourReset}")
elseif(NOT NXP_PROG)
  message(WARNING "${ColourBold}${Red}crt_emu_cm3_nxp not found! Can't program the LPC chip!${ColourReset}")
else()

  execute_process(
    COMMAND ${DFU_UTIL} -d 0x471:0xdf55 -c 0 -t 2048 -R -D ${LPCXPRESSO_PATH}/bin/LPCXpressoWIN.enc
    ERROR_QUIET
    )
  add_custom_command(TARGET ${CMAKE_PROJECT_NAME}.axf POST_BUILD
    COMMAND ${NXP_PROG} -wire=winusb -pLPC1764 -flash-load-exec=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.axf
    )
endif()
