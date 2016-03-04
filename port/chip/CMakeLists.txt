message(STATUS "Selected Controller: ${TARGET_CONTROLLER}")

if(${TARGET_CONTROLLER} MATCHES "^(lpc|LPC)17")
  set(LPC17XX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/nxp/lpc17xx)
  set(LPCOPEN_SRCPATH ${LPC17XX_PATH}/lpcopen/src)
  set(LPCOPEN_INCPATH ${LPC17XX_PATH}/lpcopen/inc)

  include_directories(${LPC17XX_PATH})

  set(PROJ_SRCS ${PROJ_SRCS}
    ${LPC17XX_PATH}/lpc17_i2c.c
    ${LPC17XX_PATH}/lpc17_spi.c
    ${LPC17XX_PATH}/lpc17_ssp.c
    ${LPC17XX_PATH}/lpc17_hpm.c
    )

  set(LPC17_SRCS ${LPC17_SRCS}
    ${LPC17XX_PATH}/sysinit.c
    ${LPC17XX_PATH}/cr_startup_lpc175x_6x.c
    )

  set(PROJ_HDRS ${PROJ_HDRS}
    ${LPC17XX_PATH}
    ${LPCOPEN_INCPATH}
    )

  set(LIBLPCOPEN_SRCS
    ${LPCOPEN_SRCPATH}/chip_17xx_40xx.c
    ${LPCOPEN_SRCPATH}/clock_17xx_40xx.c
    ${LPCOPEN_SRCPATH}/gpio_17xx_40xx.c
    ${LPCOPEN_SRCPATH}/gpioint_17xx_40xx.c
    ${LPCOPEN_SRCPATH}/i2c_17xx_40xx.c
    ${LPCOPEN_SRCPATH}/iap.c
    ${LPCOPEN_SRCPATH}/iocon_17xx_40xx.c
    ${LPCOPEN_SRCPATH}/spi_17xx_40xx.c
    ${LPCOPEN_SRCPATH}/ssp_17xx_40xx.c
    ${LPCOPEN_SRCPATH}/sysctl_17xx_40xx.c
    ${LPCOPEN_SRCPATH}/sysinit_17xx_40xx.c
    ${LPCOPEN_SRCPATH}/timer_17xx_40xx.c
    ${LPCOPEN_SRCPATH}/uart_17xx_40xx.c
    ${LPCOPEN_SRCPATH}/wwdt_17xx_40xx.c
    )

  include_directories(${LPCOPEN_INCPATH})

  set(LPC17_SRCS ${LPC17_SRCS} PARENT_SCOPE)

  set(TARGET_ARCH     "armv7-m" )
  set(TARGET_CPU      "cortex-m3")

  set(LPC17_FLAGS ${LPC17_FLAGS} -D__CODE_RED)
  set(LPC17_FLAGS ${LPC17_FLAGS} -DCORE_M3)
  set(LPC17_FLAGS ${LPC17_FLAGS} -D__USE_LPCOPEN)
  set(LPC17_FLAGS ${LPC17_FLAGS} -DNO_BOARD_LIB)
  set(LPC17_FLAGS ${LPC17_FLAGS} -D__NEWLIB__)
  set(LPC17_FLAGS ${LPC17_FLAGS} -D__LPC17XX__)
  set(LPC17_FLAGS ${LPC17_FLAGS} -mcpu=${TARGET_CPU} -mtune=${TARGET_CPU})
  set(LPC17_FLAGS ${LPC17_FLAGS} -march=${TARGET_ARCH})
  set(LPC17_FLAGS ${LPC17_FLAGS} -mthumb -mthumb-interwork -mno-sched-prolog -mapcs-frame)

  string(REGEX REPLACE ";" " " LPC17_FLAGS "${LPC17_FLAGS}")

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LPC17_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" PARENT_SCOPE)

  add_library(lpcopen STATIC ${LIBLPCOPEN_SRCS})
endif()

set(PROJ_SRCS ${PROJ_SRCS} PARENT_SCOPE)
set(PROJ_HDRS ${PROJ_HDRS} PARENT_SCOPE)
