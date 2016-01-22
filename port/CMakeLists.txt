# Build microcontroller-specific sources

if(${TARGET_CONTROLLER} MATCHES "^(lpc|LPC)17")
  set(LPC17XX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/nxp/lpc17xx)
  set(LPCOPEN_SRCPATH ${LPC17XX_PATH}/lpcopen/src)
  set(LPCOPEN_INCPATH ${LPC17XX_PATH}/lpcopen/inc)

  include_directories(${LPC17XX_PATH})

  set(PROJ_SRCS ${PROJ_SRCS}
    ${LPC17XX_PATH}/lpc17_i2c.c
    ${LPC17XX_PATH}/lpc17_spi.c
    ${LPC17XX_PATH}/lpc17_ssp.c
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

  add_library(lpcopen STATIC ${LIBLPCOPEN_SRCS})
endif()

set(PROJ_SRCS ${PROJ_SRCS} PARENT_SCOPE)
set(LPC17_SRCS ${LPC17_SRCS} PARENT_SCOPE)
set(PROJ_HDRS ${PROJ_HDRS} PARENT_SCOPE)
