#Select which microcontroller and board are being used
set(TARGET_BOARD      "afcv3_1")
set(TARGET_CONTROLLER "lpc1764")

# CPU and arch definition
if(${TARGET_CONTROLLER} STREQUAL "lpc1764")
  set(TARGET_ARCH     "armv7-m")
  set(TARGET_CPU      "cortex-m3")
endif()

# Modules definition
if(${TARGET_BOARD} MATCHES "(afcv3_1)|(afcv3_0)") # (?i) makes the regexp case insensitive
  set(TARGET_MODULES "IPMI"
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
    #"RTM"
    )
elseif(${TARGET_BOARD} MATCHES "(test)") # (?i) makes the regexp case insensitive
  set(TARGET_MODULES "IPMI"
    "FRU"
    "PAYLOAD"
    "SDR"
    "HOTSWAP"
    "TEMPERATURE_SENSOR"
    "VOLTAGE_SENSOR"
    )
endif()

