set(LPC17XX_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(LPCOPEN_SRCPATH ${LPC17XX_PATH}/lpcopen/src)
set(LPCOPEN_INCPATH ${LPC17XX_PATH}/lpcopen/inc)

include_directories(${LPC17XX_PATH})
include_directories(${PROJ_HDRS})

#Silence GCC Warnings
add_compile_options( -Wno-switch -Wno-unused-but-set-variable )

set(UCONTROLLER_SRCS
  ${LPC17XX_PATH}/cr_startup_lpc175x_6x.c
  ${LPC17XX_PATH}/sysinit.c
  )

set(UCONTROLLER_HDRS
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

  ${LPC17XX_PATH}/lpc17_i2c.c
  ${LPC17XX_PATH}/lpc17_spi.c
  ${LPC17XX_PATH}/lpc17_ssp.c
  ${LPC17XX_PATH}/lpc17_hpm.c
  )

if( UART_RINGBUFFER )
  set(LIBLPCOPEN_SRCS ${LIBLPCOPEN_SRCS} ${LPCOPEN_SRCPATH}/ring_buffer.c ${LPC17XX_PATH}/lpc17_uartrb.c)
  set(MODULES_FLAGS ${MODULES_FLAGS} -DUART_RINGBUFFER PARENT_SCOPE)
else()
  set(LIBLPCOPEN_SRCS ${LIBLPCOPEN_SRCS} ${LPC17XX_PATH}/lpc17_uart.c)
endif()

include_directories(${LPCOPEN_INCPATH})

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

set(UCONTROLLER_SRCS ${UCONTROLLER_SRCS} PARENT_SCOPE)
set(UCONTROLLER_HDRS ${UCONTROLLER_HDRS} PARENT_SCOPE)

add_library(lpcopen STATIC ${LIBLPCOPEN_SRCS})

add_dependencies(lpcopen FreeRTOS)
