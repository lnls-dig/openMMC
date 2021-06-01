#Select which microcontroller and board are being used
#if (NOT TARGET_CONTROLLER)
set(TARGET_CONTROLLER "LPC1776" CACHE STRING "Target MMC Controller")
#endif()

if (NOT TARGET_BOARD_NAME)
  set(TARGET_BOARD_NAME "Sayma" CACHE STRING "Board Name")
endif()

#List all modules used by this board
set(TARGET_MODULES
  "FRU"
  "PAYLOAD"
  "SDR"
  "FPGA_SPI"
  "HOTSWAP_SENSOR"
  "EEPROM_AT24MAC"
  "EEPROM_24XX64"
  "LM75"
  "MAX6642"
  "ADT7420"
  "INA219"
  #"HPM"
  "RTM"
  "UART_DEBUG"
  "SENSORS"
  "CLI"
  "BOARD_CONFIG"
  "SYSUTILS"
  )

if (NOT DISABLE_WATCHDOG)
  list(APPEND TARGET_MODULES "WATCHDOG")
endif()

set(BOARD_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#Include the modules sources
##Sensors SDR list
set( PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/sdr_list.c )
set( PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/i2c_mapping.c )
set( PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/ipmi_oem.c )

if (";${TARGET_MODULES};" MATCHES ";PAYLOAD;")
  set(PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/payload.c)
  set(MODULES_FLAGS "${MODULES_FLAGS} -DMODULE_PAYLOAD")
endif()

if (";${TARGET_MODULES};" MATCHES ";CLI;")
  set(PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/cli_commands.c)
endif()

if (";${TARGET_MODULES};" MATCHES ";BOARD_CONFIG;")
  set(PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/board_config.c)
  set(MODULES_FLAGS "${MODULES_FLAGS} -DMODULE_BOARD_CONFIG")
endif()

set(PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/board_led.c)
set(PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/board_ipmb.c)

if (";${TARGET_MODULES};" MATCHES ";FRU;")
  set(PROJ_SRCS ${PROJ_SRCS} ${BOARD_PATH}/board_fru.c)
  set(MODULES_FLAGS "${MODULES_FLAGS} -DMODULE_FRU")
endif()

#Set the variables in the main scope
set(TARGET_MODULES ${TARGET_MODULES} PARENT_SCOPE)
set(MODULES_FLAGS ${MODULES_FLAGS} PARENT_SCOPE)
set(PROJ_SRCS ${PROJ_SRCS} PARENT_SCOPE)
set(PROJ_HDRS ${PROJ_HDRS} ${BOARD_PATH})
set(PROJ_HDRS ${PROJ_HDRS} PARENT_SCOPE)
