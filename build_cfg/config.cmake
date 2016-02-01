#Select which microcontroller and board are being used
set(TARGET_CONTROLLER "LPC1764")

set(TARGET_BOARD_NAME  "AFC3_1")
set(TARGET_BOARD_MAJOR 3)
set(TARGET_BOARD_MINOR 1)

#Normalize strings to upper case
string(TOUPPER "${TARGET_CONTROLLER}" TARGET_CONTROLLER_UPPER)
set(TARGET_CONTROLLER "${TARGET_CONTROLLER_UPPER}")
string(TOUPPER "${TARGET_BOARD_NAME}" TARGET_BOARD_UPPER)
set(TARGET_BOARD_NAME "${TARGET_BOARD_UPPER}")

# CPU and arch definition
if(${TARGET_CONTROLLER} STREQUAL "LPC1764")
  set(TARGET_ARCH     "armv7-m")
  set(TARGET_CPU      "cortex-m3")
endif()

# Modules definition
if(${TARGET_BOARD_NAME} MATCHES "^(AFC)" AND ${TARGET_BOARD_MAJOR} EQUAL "3")
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
    #"RTM"
    )
elseif(${TARGET_BOARD_NAME} MATCHES "TEST")
  set(TARGET_MODULES
    "FRU"
    "PAYLOAD"
    "SDR"
    "HOTSWAP"
    "TEMPERATURE_SENSOR"
    "VOLTAGE_SENSOR"
    )
endif()

if(${TARGET_CONTROLLER} MATCHES "^LPC17")
  add_definitions(-D__CODE_RED)
  add_definitions(-DCORE_M3)
  add_definitions(-D__USE_LPCOPEN)
  add_definitions(-DNO_BOARD_LIB)
  add_definitions(-D__NEWLIB__)
  add_definitions(-D__LPC17XX__)
endif()
