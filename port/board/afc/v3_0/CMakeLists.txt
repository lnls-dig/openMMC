#Select which microcontroller and board are being used
set(TARGET_CONTROLLER "LPC1764" CACHE STRING "Target MMC Controller" FORCE)

set(TARGET_BOARD_NAME  "AFC 3.0" CACHE STRING "Board Name" FORCE)

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
  "EEPROM_AT24MAC"
  "HOTSWAP_SENSOR"
  "TEMPERATURE_SENSOR"
  "VOLTAGE_SENSOR"
  "CURRENT_SENSOR"
  "HPM"
  )

set(BOARD_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#Include the modules sources
##Sensors SDR list
set(PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/sdr_list.c)
set(PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/i2c_mapping.c)

#Extra definitions
if (FRU_WRITE_EEPROM)
  message(STATUS "FRU EEPROM will be written if no valid data is found!")
  set(MODULES_FLAGS "${MODULES_FLAGS} -DFRU_WRITE_EEPROM")
  set(FRU_WRITE_EEPROM false CACHE BOOL "Set this flag to enable writing the board ID on the EEPROM" FORCE)
endif()

#Set the variables in the main scope
set(TARGET_MODULES ${TARGET_MODULES} PARENT_SCOPE)
set(MODULES_FLAGS ${MODULES_FLAGS} PARENT_SCOPE)
set(PROJ_SRCS ${PROJ_SRCS} PARENT_SCOPE)
set(PROJ_HDRS ${PROJ_HDRS} ${BOARD_PATH})
set(PROJ_HDRS ${PROJ_HDRS} PARENT_SCOPE)
