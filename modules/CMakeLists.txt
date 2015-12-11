set(MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(PROJ_HDRS ${PROJ_HDRS} ${MODULE_PATH} )

if (";${TARGET_MODULES};" MATCHES ";IPMI;")
  set(PROJ_SRCS ${PROJ_SRCS}
    ${MODULE_PATH}/i2c.c
    ${MODULE_PATH}/ipmb.c
    ${MODULE_PATH}/ipmi.c
    )
endif()

if (";${TARGET_MODULES};" MATCHES ";FRU;")
  set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/fru.c )
endif()

if (";${TARGET_MODULES};" MATCHES ";PAYLOAD;")
  set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/payload.c)
endif()

if (";${TARGET_MODULES};" MATCHES ";SDR;")
  set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/sdr.c )
endif()

if (";${TARGET_MODULES};" MATCHES ";JTAG_SWITCH;")
  set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/jtag.c )
endif()

if (";${TARGET_MODULES};" MATCHES ";CLOCK_SWITCH;")
  set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/adn4604.c )
endif()

if (";${TARGET_MODULES};" MATCHES ";FPGA_SPI;")
  set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/fpga_spi.c )
endif()

if (";${TARGET_MODULES};" MATCHES ";DAC_AD84XX;")
  set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/ad84xx.c )
endif()

if (";${TARGET_MODULES};" MATCHES ";WATCHDOG;")
  set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/watchdog.c )
endif()

if (";${TARGET_MODULES};" MATCHES ";SENSOR;")
  add_subdirectory(sensors)
endif()

set(PROJ_SRCS ${PROJ_SRCS} PARENT_SCOPE)
set(PROJ_HDRS ${PROJ_HDRS} PARENT_SCOPE)
