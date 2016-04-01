#Select which microcontroller and board are being used
set(TARGET_CONTROLLER "LPC1764" CACHE STRING "Target MMC Controller")

set(TARGET_BOARD_NAME  "AFC3_1" CACHE STRING "Board Name")
set(TARGET_BOARD_MAJOR 3 CACHE STRING "Board Major Version")
set(TARGET_BOARD_MINOR 1 CACHE STRING "Board Minor Version")

#List all modules used by this board
set(TARGET_MODULES
  "FRU"
  "PAYLOAD"
  "SDR"
  "WATCHDOG"
  "JTAG_SWITCH"
  "CLOCK_SWITCH"
  "FPGA_SPI"
  "DAC_AD84XX"
  "HOTSWAP_SENSOR"
  "TEMPERATURE_SENSOR"
  "VOLTAGE_SENSOR"
  "HPM"
  "PCA9554"
  "RTM"
  )

set(BOARD_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#Include the modules sources
##Sensors SDR list
set( PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/sdr_list.c )

#Extra definitions
if (WRITE_ID_EEPROM)
  message(STATUS "EEPROM ID writing enabled!")
  add_definitions(-DWRITE_ID_EEPROM)
  set(WRITE_ID_EEPROM false CACHE BOOL "Set this flag to enable writing the board ID on the EEPROM" FORCE)
endif()

#Set the variables in the main scope
set(TARGET_MODULES ${TARGET_MODULES} PARENT_SCOPE)
set(MODULES_FLAGS ${MODULES_FLAGS} PARENT_SCOPE)
set(PROJ_SRCS ${PROJ_SRCS} PARENT_SCOPE)
set(PROJ_HDRS ${PROJ_HDRS} ${BOARD_PATH})
set(PROJ_HDRS ${PROJ_HDRS} PARENT_SCOPE)
