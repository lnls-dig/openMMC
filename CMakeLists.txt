# Define minimal required version of CMake.
cmake_minimum_required(VERSION 2.8.8)

#Include text color definitions
include( ${CMAKE_SOURCE_DIR}/toolchain/colors.cmake )

#Prevent in-source builds
include( ${CMAKE_SOURCE_DIR}/toolchain/PreventInSourceBuilds.cmake )

include( ${CMAKE_SOURCE_DIR}/toolchain/cmake-modules/GetGitRevisionDescription.cmake )

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

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: none Debug Release."
    FORCE)
endif()
message( STATUS "Build type: ${CMAKE_BUILD_TYPE}" )

# Include board specific code
set(CMAKE_ERROR_FLAGS "-Wall -Wextra -Wpointer-arith -Wno-packed-bitfield-compat -Wno-unused-parameter -Wno-missing-field-initializers")

# Build specific flags
set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} ${CMAKE_ERROR_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG     "-O0 -g3 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE   "-Os")

# When we break up long strings in CMake we get semicolon
# separated lists, undo this here...
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" CACHE STRING "")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "")

set(PROJ_HDRS ${CMAKE_SOURCE_DIR} )

add_subdirectory(port/board)
add_subdirectory(FreeRTOS)
add_subdirectory(port/ucontroller)
add_subdirectory(boot)
add_subdirectory(modules)

include_directories( ${UCONTROLLER_HDRS} ${PROJ_HDRS} )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTARGET_CONTROLLER=${TARGET_CONTROLLER} -DTARGET_BOARD_NAME=\"${TARGET_BOARD_NAME}\" ")

if(BENCH_TEST)
  set(MODULES_FLAGS "${MODULES_FLAGS} -DBENCH_TEST")
  message( STATUS "${Magenta}Bench mode activated! ${ColourReset}")
endif()

# Get Git information
get_git_head_revision(GIT_REFSPEC GIT_SHA1 "--abbrev=4 --tags")
configure_file("${CMAKE_SOURCE_DIR}/modules/GitSHA1.c.in" "${CMAKE_SOURCE_DIR}/modules/GitSHA1.c" @ONLY)
list(APPEND PROJ_SRCS "${CMAKE_SOURCE_DIR}/modules/GitSHA1.c" "${CMAKE_SOURCE_DIR}/modules/GitSHA1.h")

# Libraries path
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

## Create executable
add_executable(${CMAKE_PROJECT_NAME} ${UCONTROLLER_SRCS} ${PROJ_SRCS})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${MODULES_FLAGS})

add_executable(bootloader ${BOOT_SRCS})

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
target_link_libraries(${CMAKE_PROJECT_NAME} FreeRTOS c gcc m lpcopen)
target_link_libraries(bootloader gcc c m lpcopen)

##Generate binary file
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
if(NOT DEBUG_PROBE)
  set(DEBUG_PROBE "LPCLink")
endif()

string(TOUPPER ${DEBUG_PROBE} DEBUG_PROBE_UPPERCASE)

if(${DEBUG_PROBE_UPPERCASE} MATCHES "^(LPCLINK)$")
  set(DFU_DEVICE "0x0471:0xDF55")
  set(DFU_FIRMWARE "LPCXpressoWIN.enc")
  set(NXP_PROG_NAME "crt_emu_cm3_nxp")
  set(NXP_PROG_FLAGS "-wire=winusb")
elseif(${DEBUG_PROBE_UPPERCASE} MATCHES "^(LPCLINK2)$")
  set(DFU_DEVICE "0x1FC9:0x000C")
  set(NXP_PROG_NAME "crt_emu_cm_redlink")
  set(DFU_FIRMWARE "LPC432x_CMSIS_DAP_V5_173.bin.hdr")
  set(NXP_PROG_FLAGS "")
else()
  message(FATAL_ERROR "${DEBUG_PROBE} not implemented!")
endif()

message(STATUS "Selected debug probe: ${DEBUG_PROBE}")

set(LPCXPRESSO_INSTALL_PATH /usr/local/lpcxpresso/lpcxpresso/bin)

find_program(DFU_UTIL dfu-util)
if(NOT DFU_UTIL)
  message(WARNING "${ColourBold}${Red}Dfu-util not found! Can't boot the programmer board!${ColourReset}")
endif()

find_program(NXP_PROG ${NXP_PROG_NAME} PATHS ${LPCXPRESSO_INSTALL_PATH})
if(NOT NXP_PROG)
  message(WARNING "${ColourBold}${Red}${NXP_PROG_NAME} not found! Can't program the controller!${ColourReset}")
else()
  get_filename_component( DFU_FIRMWARE_PATH ${NXP_PROG} DIRECTORY )
endif()

# Program the chip (available only for LPC1764 so far)
add_custom_target(program_boot
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
  DEPENDS bootloader
  #Boot the programmer
  COMMAND ${DFU_UTIL} -d ${DFU_DEVICE} -c 0 -t 2048 -R -D ${DFU_FIRMWARE_PATH}/${DFU_FIRMWARE} || true
  #Write bootloader firmware
  COMMAND ${NXP_PROG} -p${TARGET_CONTROLLER} -g ${NXP_PROG_FLAGS} -flash-load=bootloader.bin -load-base=0x0000
  )

add_custom_target(program_app
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
  DEPENDS ${CMAKE_PROJECT_NAME}
  #Boot the programmer
  COMMAND ${DFU_UTIL} -d ${DFU_DEVICE} -c 0 -t 2048 -R -D ${DFU_FIRMWARE_PATH}/${DFU_FIRMWARE} || true
  #Write application firmware
  COMMAND ${NXP_PROG} -p${TARGET_CONTROLLER} -g ${NXP_PROG_FLAGS} -flash-load-exec=${CMAKE_PROJECT_NAME}.bin -load-base=0x2000
  )

add_custom_target(program_all
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
  DEPENDS ${CMAKE_PROJECT_NAME} bootloader
  COMMAND ${DFU_UTIL} -d ${DFU_DEVICE} -c 0 -t 2048 -R -D ${DFU_FIRMWARE_PATH}/${DFU_FIRMWARE} || true
  #Write application and bootloader firmwares
  COMMAND ${NXP_PROG} -p${TARGET_CONTROLLER} -g  ${NXP_PROG_FLAGS} -flash-mass-erase
  COMMAND sleep 0.5
  COMMAND make -C ${CMAKE_BINARY_DIR} program_boot
  COMMAND sleep 0.5
  COMMAND make -C ${CMAKE_BINARY_DIR} program_app
  )

add_custom_target(full_binary
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
  DEPENDS ${CMAKE_PROJECT_NAME} bootloader
  #Create a pad file with the total size of the bootloader (0x2000)
  COMMAND dd if=/dev/zero bs=1 count=8192 | tr \"\\000\" \"\\377\" > bootloader_pad.bin
  #Add padding bytes to the bootloader binary
  COMMAND dd if=bootloader.bin of=bootloader_pad.bin conv=notrunc
  #Concatenate bootloader and app binaries
  COMMAND cat bootloader_pad.bin ${CMAKE_PROJECT_NAME}.bin > openMMC_full.bin

  #Program full_binary (for tests only)
  #COMMAND ${DFU_UTIL} -d ${DFU_DEVICE} -c 0 -t 2048 -R -D ${DFU_FIRMWARE_PATH}/${DFU_FIRMWARE} || true
  #COMMAND ${NXP_PROG} ${NXP_PROG_FLAGS} -flash-load-exec=openMMC_full.bin -load-base=0x0
  )
