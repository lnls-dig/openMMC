set(MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(PROJ_HDRS ${PROJ_HDRS} ${MODULE_PATH} )

set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/utils.c)
set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/board_version.c)
set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/led.c)
set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/ipmb.c ${MODULE_PATH}/ipmi.c)

message(STATUS "Selected modules to compile: ${TARGET_MODULES}")

if (";${TARGET_MODULES};" MATCHES ";FRU;")
  set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/fru.c )
  set(MODULES_FLAGS "${MODULES_FLAGS} -DMODULE_FRU")
 endif()

if (";${TARGET_MODULES};" MATCHES ";PAYLOAD;")
  set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/payload.c)
  set(MODULES_FLAGS "${MODULES_FLAGS} -DMODULE_PAYLOAD")
endif()

if (";${TARGET_MODULES};" MATCHES ";SDR;")
  set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/sdr.c )
  set(MODULES_FLAGS "${MODULES_FLAGS} -DMODULE_SDR")
endif()

if (";${TARGET_MODULES};" MATCHES ";WATCHDOG;")
  set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/watchdog.c )
  set(MODULES_FLAGS "${MODULES_FLAGS} -DMODULE_WATCHDOG")
endif()

if (";${TARGET_MODULES};" MATCHES ";HPM;")
  set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/hpm.c )
  set(MODULES_FLAGS "${MODULES_FLAGS} -DMODULE_HPM")
  if (";${TARGET_MODULES};" MATCHES ";PAYLOAD;")
    set(PROJ_SRCS ${PROJ_SRCS} ${MODULE_PATH}/flash_spi.c )
    set(MODULES_FLAGS "${MODULES_FLAGS} -DMODULE_FLASH_SPI")
  endif()
 endif()

if (";${TARGET_MODULES};" MATCHES "SENSOR")
  set(MODULES_FLAGS "${MODULES_FLAGS} -DMODULE_SENSORS")
  add_subdirectory(sensors)
endif()

#Modules dependencies check
if(${TARGET_BOARD_NAME} MATCHES "^(AFC)")
  if (("${MODULES_FLAGS}" MATCHES "-DMODULE_PAYLOAD") AND NOT ("${MODULES_FLAGS}" MATCHES "-DMODULE_DAC_AD84XX"))
    message(WARNING "${Red}[AFC] Payload module is being used but the DAC_AD84XX was not included in this build. The VADJ reference will not be configured! ${ColourReset}")
  endif()
endif()

if(NOT ("${MODULES_FLAGS}" MATCHES "-DMODULE_HOTSWAP"))
  message(FATAL_ERROR "${Red}[ERROR] Mandatory Hotswap sensor module not included!${ColourReset}")
endif()

set(PROJ_SRCS ${PROJ_SRCS} PARENT_SCOPE)
set(PROJ_HDRS ${PROJ_HDRS} PARENT_SCOPE)
set(MODULES_FLAGS ${MODULES_FLAGS} PARENT_SCOPE)
