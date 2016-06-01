# Define minimal required version of CMake.
cmake_minimum_required(VERSION 2.8.7)

#Include text color definitions
include( ${CMAKE_SOURCE_DIR}/toolchain/colors.cmake )

#Prevent in-source builds
include( ${CMAKE_SOURCE_DIR}/toolchain/PreventInSourceBuilds.cmake )

# Project definition
project(openMMC C CXX ASM)

##
# CMake environment settings
#

#Find a toolchain file
if(NOT CMAKE_TOOLCHAIN_FILE)
  message(WARNING "${Yellow}No toolchain configuration file specified. Using default option!${ColourReset}")

  include(${CMAKE_SOURCE_DIR}/toolchain/toolchain-arm-none-eabi.cmake)

endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build_cfg")

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
    "Choose the type of build, options are: none Debug Release Trace."
    FORCE)
endif()
message( STATUS "Build type: ${CMAKE_BUILD_TYPE}" )

# Include board specific code
set(CMAKE_ERROR_FLAGS "-Wall -Wextra -Wpointer-arith -Wno-packed-bitfield-compat -Wno-unused-parameter -Wno-missing-field-initializers")

# Build specific flags
set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} ${CMAKE_ERROR_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS} -O0 -g3 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS} -O3")

# When we break up long strings in CMake we get semicolon
# separated lists, undo this here...
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" CACHE STRING "")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "")

set(PROJ_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/main_mmc.c)
add_subdirectory(port/board)
add_subdirectory(port/ucontroller)
add_subdirectory(FreeRTOS)
add_subdirectory(boot)
add_subdirectory(modules)
if(CMAKE_BUILD_TYPE STREQUAL "Trace")
  add_subdirectory(trace)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTARGET_CONTROLLER=${TARGET_CONTROLLER} -DTARGET_BOARD_NAME=\"${TARGET_BOARD_NAME}\" ")

if(BENCH_TEST)
  set(MODULES_FLAGS "${MODULES_FLAGS} -DBENCH_TEST")
endif()

# Libraries path
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

## Create executable
add_executable(${CMAKE_PROJECT_NAME} ${LPC17_SRCS} ${PROJ_SRCS})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${MODULES_FLAGS})

add_executable(bootloader ${BOOT_SRCS} ${LPC17_SRCS})

# Linker flags
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
  SUFFIX ".axf"
  LINK_FLAGS "-T ${CMAKE_SOURCE_DIR}/linker/${TARGET_CONTROLLER}_app.ld -Wl,-Map=${CMAKE_SOURCE_DIR}/linker/${TARGET_CONTROLLER}_app.map" )

set_target_properties(bootloader PROPERTIES
  SUFFIX ".axf"
  LINK_FLAGS "-T ${CMAKE_SOURCE_DIR}/linker/${TARGET_CONTROLLER}_boot.ld -Wl,-Map=${CMAKE_SOURCE_DIR}/linker/${TARGET_CONTROLLER}_boot.map")


# Headers path
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${PROJ_HDRS})
target_include_directories(bootloader PUBLIC ${PROJ_HDRS})
# Link libraries
target_link_libraries(${CMAKE_PROJECT_NAME} gcc c m FreeRTOS lpcopen)
target_link_libraries(bootloader gcc c m lpcopen)

##Generate .hex file

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.axf ${CMAKE_PROJECT_NAME}.bin
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  COMMENT "Converting the AXF output to a binary file"
  )

add_custom_command(TARGET bootloader POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary bootloader.axf bootloader.bin
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  COMMENT "Converting the AXF output to a binary file"
  )


##Program command
set(LPCXPRESSO_PATH /usr/local/lpcxpresso_7.8.0_426/lpcxpresso/)

find_program(DFU_UTIL dfu-util)
find_program(NXP_PROG crt_emu_cm3_nxp PATHS ${LPCXPRESSO_PATH}/bin NO_DEFAULT_PATH)

if(NOT DFU_UTIL)
  message(WARNING "${ColourBold}${Red}Dfu-util not found! Can't boot the LPCLink board!${ColourReset}")
elseif(NOT NXP_PROG)
  message(WARNING "${ColourBold}${Red}crt_emu_cm3_nxp not found! Can't program the LPC chip!${ColourReset}")
else()

  # Program the chip (available only for LPC1764 so far)
  add_custom_target(program_boot
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
    DEPENDS bootloader
    #Boot the programmer
    COMMAND ${DFU_UTIL} -d 0x471:0xdf55 -c 0 -t 2048 -R -D ${LPCXPRESSO_PATH}/bin/LPCXpressoWIN.enc || true
    #Write bootloader firmware
    COMMAND ${NXP_PROG} -wire=winusb -p${TARGET_CONTROLLER} -g -flash-load=bootloader.bin -load-base=0x0000
    )

  add_custom_target(program_app
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
    DEPENDS ${CMAKE_PROJECT_NAME}
    #Boot the programmer
    COMMAND ${DFU_UTIL} -d 0x471:0xdf55 -c 0 -t 2048 -R -D ${LPCXPRESSO_PATH}/bin/LPCXpressoWIN.enc || true
    #Write application firmware
    COMMAND ${NXP_PROG} -wire=winusb -p${TARGET_CONTROLLER} -g -flash-load-exec=${CMAKE_PROJECT_NAME}.bin -load-base=0x2000
    )

  add_custom_target(program_all
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
    DEPENDS ${CMAKE_PROJECT_NAME} bootloader
    COMMAND ${DFU_UTIL} -d 0x471:0xdf55 -c 0 -t 2048 -R -D ${LPCXPRESSO_PATH}/bin/LPCXpressoWIN.enc || true
    #Write application and bootloader firmwares
    COMMAND ${NXP_PROG} -wire=winusb -p${TARGET_CONTROLLER} -flash-mass-erase
    COMMAND make -C ${CMAKE_SOURCE_DIR} program_boot
    COMMAND make -C ${CMAKE_SOURCE_DIR} program_app
    )
endif()
