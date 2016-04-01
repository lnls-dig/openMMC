# Build FreeRTOS library

set(FREERTOS_INCDIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(FREERTOS_INC ${FREERTOS_INCDIR})

#Port files
if(${TARGET_CONTROLLER} MATCHES "^(lpc|LPC)17")
  set(FREERTOS_INC ${FREERTOS_INC} ${CMAKE_CURRENT_SOURCE_DIR}/portable/GCC/ARM_CM3)
  set(LIBFREERTOS_SRCS ${LIBFREERTOS_SRCS}
    portable/GCC/ARM_CM3/port.c
    portable/MemMang/heap_4.c
    )
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
  )

include_directories(${PROJ_HDRS})
include_directories(${FREERTOS_INC})
set(PROJ_HDRS ${PROJ_HDRS} ${FREERTOS_INC} PARENT_SCOPE)

add_library(FreeRTOS STATIC ${LIBFREERTOS_SRCS})
