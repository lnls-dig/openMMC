# Build FreeRTOS library

set(FREERTOS_INCDIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(FREERTOS_INC ${FREERTOS_INCDIR})

#Include FreeRTOS Trace module sources
if(FREERTOS_TRACE)
  add_subdirectory(trace)
endif()

#Port files
if(${TARGET_CONTROLLER} MATCHES "^(lpc|LPC)17")
  set(FREERTOS_INC ${FREERTOS_INC} ${CMAKE_CURRENT_SOURCE_DIR}/portable/GCC/ARM_CM3)
  set(LIBFREERTOS_SRCS ${LIBFREERTOS_SRCS}
    portable/GCC/ARM_CM3/port.c
    portable/MemMang/heap_4.c
    )
  set(TARGET_ARCH     "armv7-m" )
  set(TARGET_CPU      "cortex-m3")
  set(FREERTOS_FLAGS ${FREERTOS_FLAGS} -mcpu=${TARGET_CPU} -mtune=${TARGET_CPU})
  set(FREERTOS_FLAGS ${FREERTOS_FLAGS} -march=${TARGET_ARCH})
  set(FREERTOS_FLAGS ${FREERTOS_FLAGS} -mthumb -mthumb-interwork -mno-sched-prolog -mapcs-frame)
else()
  message(FATAL_ERROR "${BoldRed}The specified controller doesn't have a supported FreeRTOS port!${ColourReset}")
endif()

#FreeRTOS common files
set(LIBFREERTOS_SRCS ${LIBFREERTOS_SRCS}
  list.c
  queue.c
  timers.c
  tasks.c
  event_groups.c
  FreeRTOS_CLI.c
  )

include_directories(${UCONTROLLER_HDRS})

string(REGEX REPLACE ";" " " FREERTOS_FLAGS "${FREERTOS_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FREERTOS_FLAGS}")

add_library(FreeRTOS STATIC ${LIBFREERTOS_SRCS})
target_include_directories(FreeRTOS PUBLIC ${FREERTOS_INC})
