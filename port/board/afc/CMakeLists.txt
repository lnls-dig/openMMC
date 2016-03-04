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
  )

set(BOARD_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#Include the modules sources

if (";${TARGET_MODULES};" MATCHES ";JTAG_SWITCH;")
  set(PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/jtag.c )
  set(MODULES_FLAGS "${MODULES_FLAGS} -DMODULE_JTAG_SWITCH")
endif()

if (";${TARGET_MODULES};" MATCHES ";CLOCK_SWITCH;")
  set(PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/adn4604.c )
  set(MODULES_FLAGS "${MODULES_FLAGS} -DMODULE_CLOCK_SWITCH")
endif()

if (";${TARGET_MODULES};" MATCHES ";FPGA_SPI;")
  set(PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/fpga_spi.c )
  set(MODULES_FLAGS "${MODULES_FLAGS} -DMODULE_FPGA_SPI")
endif()

if (";${TARGET_MODULES};" MATCHES ";DAC_AD84XX;")
  set(PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/ad84xx.c )
  set(MODULES_FLAGS "${MODULES_FLAGS} -DMODULE_DAC_AD84XX")
endif()

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
