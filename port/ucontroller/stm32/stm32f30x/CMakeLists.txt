set(STM32F30X_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(SYSLIB_SRCPATH ${STM32F30X_PATH}/system/lib)
set(SYS_INCPATH ${STM32F30X_PATH}/system/include)
set(CMSIS_INCPATH ${STM32F30X_PATH}/system/include/cmsis)
set(STDPERIPH_INCPATH ${STM32F30X_PATH}/system/include/stdperiph)

#Silence GCC Warnings
add_compile_options( -Wno-switch -Wno-unused-but-set-variable )

set(UCONTROLLER_SRCS
   ${STM32F30X_PATH}/stm32_assert.c
   ${STM32F30X_PATH}/stm32_pincfg.c
   ${STM32F30X_PATH}/stm32_uart.c
   ${STM32F30X_PATH}/stm32_gpio.c
   ${STM32F30X_PATH}/stm32_timer.c
   ${STM32F30X_PATH}/stm32_i2c.c
  )

set(UCONTROLLER_HDRS
  ${STM32F30X_PATH}
  ${SYS_INCPATH}
  ${CMSIS_INCPATH}
  ${STDPERIPH_INCPATH}
  )

set(LIBSTM_SRCS
  ${SYSLIB_SRCPATH}/startup_stm32f30x.s
  ${SYSLIB_SRCPATH}/system_stm32f30x.c
  ${SYSLIB_SRCPATH}/_syscalls.c
  ${SYSLIB_SRCPATH}/_sbrk.c
  ${SYSLIB_SRCPATH}/stm32f30x_flash.c
  ${SYSLIB_SRCPATH}/stm32f30x_rtc.c
  ${SYSLIB_SRCPATH}/stm32f30x_gpio.c
  ${SYSLIB_SRCPATH}/stm32f30x_hrtim.c
  ${SYSLIB_SRCPATH}/stm32f30x_usart.c
  ${SYSLIB_SRCPATH}/stm32f30x_comp.c
  ${SYSLIB_SRCPATH}/stm32f30x_exti.c
  ${SYSLIB_SRCPATH}/stm32f30x_adc.c
  ${SYSLIB_SRCPATH}/stm32f30x_fmc.c
  ${SYSLIB_SRCPATH}/stm32f30x_can.c
  ${SYSLIB_SRCPATH}/stm32f30x_dbgmcu.c
  ${SYSLIB_SRCPATH}/stm32f30x_rcc.c
  ${SYSLIB_SRCPATH}/stm32f30x_crc.c
  ${SYSLIB_SRCPATH}/stm32f30x_wwdg.c
  ${SYSLIB_SRCPATH}/stm32f30x_iwdg.c
  ${SYSLIB_SRCPATH}/stm32f30x_misc.c
  ${SYSLIB_SRCPATH}/stm32f30x_tim.c
  ${SYSLIB_SRCPATH}/stm32f30x_dma.c
  ${SYSLIB_SRCPATH}/stm32f30x_dac.c
  ${SYSLIB_SRCPATH}/stm32f30x_syscfg.c
  ${SYSLIB_SRCPATH}/stm32f30x_spi.c
  ${SYSLIB_SRCPATH}/stm32f30x_pwr.c
  ${SYSLIB_SRCPATH}/stm32f30x_opamp.c
  ${SYSLIB_SRCPATH}/stm32f30x_i2c.c
  )


set(TARGET_MCU      "STM32F303xE" )
set(TARGET_CPU      "cortex-m4")

set(STM32F30X_FLAGS ${STM32F30X_FLAGS} -mcpu=${TARGET_CPU} -mtune=${TARGET_CPU})

set(STM32F30X_FLAGS ${STM32F30X_FLAGS} -mlittle-endian -mthumb -mthumb-interwork -Wl,--gc-sections -msoft-float -ffunction-sections -fdata-sections)
set(STM32F30X_FLAGS ${STM32F30X_FLAGS} -D${TARGET_MCU})
set(STM32F30X_FLAGS ${STM32F30X_FLAGS} -DUSE_STDPERIPH_DRIVER)

string(REGEX REPLACE ";" " " STM32F30X_FLAGS "${STM32F30X_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${STM32F30X_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" PARENT_SCOPE)

set(UCONTROLLER_SRCS ${UCONTROLLER_SRCS} PARENT_SCOPE)
set(UCONTROLLER_HDRS ${UCONTROLLER_HDRS} PARENT_SCOPE)

add_library(libstm STATIC ${LIBSTM_SRCS})

add_dependencies(libstm FreeRTOS)
target_link_libraries(libstm PUBLIC FreeRTOS)
target_include_directories(libstm PUBLIC ${SYS_INCPATH})
target_include_directories(libstm PUBLIC ${STM32F30X_PATH})
target_include_directories(libstm PUBLIC ${CMSIS_INCPATH})
target_include_directories(libstm PUBLIC ${STDPERIPH_INCPATH})

