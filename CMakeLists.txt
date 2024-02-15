# Define minimal required version of CMake.
cmake_minimum_required(VERSION 3.0.0)

option(DISABLE_WATCHDOG "Disable watchdog module to aid debugging" OFF)

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
  message(NOTICE "No toolchain configuration file specified. Using default option!")

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
set(CMAKE_ERROR_FLAGS "-Wall -Wextra -Wpointer-arith -Wno-packed-bitfield-compat -Wno-unused-parameter -Wno-missing-field-initializers -Werror=int-conversion -Werror=stringop-overflow -Werror=maybe-uninitialized -Werror=incompatible-pointer-types")

# Build specific flags
set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} ${CMAKE_ERROR_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG     "-Og -g3 -DDEBUG")
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
set(UCONTROLLER_APP_LD_SCRIPT "")

add_subdirectory(port/board)
add_subdirectory(port/ucontroller)
add_subdirectory(FreeRTOS)
add_subdirectory(modules)

include_directories( ${UCONTROLLER_HDRS} ${PROJ_HDRS} )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTARGET_CONTROLLER=${TARGET_CONTROLLER} -DTARGET_BOARD_NAME=\"${TARGET_BOARD_NAME}\" ")

# Get Git information
git_describe(GIT_TAG "--tags")
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
configure_file("${CMAKE_SOURCE_DIR}/modules/GitSHA1.c.in" "${CMAKE_SOURCE_DIR}/modules/GitSHA1.c" @ONLY)
list(APPEND PROJ_SRCS "${CMAKE_SOURCE_DIR}/modules/GitSHA1.c" "${CMAKE_SOURCE_DIR}/modules/GitSHA1.h")

# Libraries path
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

## Create executable
add_executable(${CMAKE_PROJECT_NAME} ${UCONTROLLER_SRCS} ${PROJ_SRCS})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${MODULES_FLAGS})

if(${BOARD} MATCHES "^(afc)")
  set(PROJ_LIBS lpcopen)
else()
  message(FATAL_ERROR "${BOARD} not supported!")
endif()

# Linker flags
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
  SUFFIX ".elf"
  LINK_FLAGS "-T ${UCONTROLLER_APP_LD_SCRIPT} -Wl,-Map=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_CONTROLLER}_app.map"
  )

# Headers path
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${PROJ_HDRS})
# Link libraries
target_link_libraries(${CMAKE_PROJECT_NAME} FreeRTOS gcc c ${PROJ_LIBS})

##Generate binary file
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  COMMENT "Converting the ELF output to a binary file"
  )

##Generate hpm files if bin2hpm is installed

find_program(BIN2HPM NAMES "bin2hpm")
if(BIN2HPM)
  add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND bin2hpm -c 1 -n -m 0x315A -p 0x00 ${CMAKE_PROJECT_NAME}.bin -o ${CMAKE_PROJECT_NAME}.hpm
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Creating HPM file from binary"
  )
  message(STATUS "bin2hpm found in the $PATH, .hpm files will be generated automatically.")
else()
  message(NOTICE "bin2hpm not found in the $PATH, .hpm files will not be generated.")
endif()

include( ${CMAKE_SOURCE_DIR}/probe/openocd.cmake )
