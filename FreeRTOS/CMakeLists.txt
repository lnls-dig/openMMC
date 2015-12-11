# Build FreeRTOS library

set(FREERTOS_INC ${CMAKE_CURRENT_BINARY_DIR}/include)

include_directories(${FREERTOS_INC})

set(PROJ_HDRS ${PROJ_HDRS} ${FREERTOS_INC} PARENT_SCOPE)

set(LIBFREERTOS_SRCS
  list.c
  queue.c
  timers.c
  port.c
  tasks.c
  heap_4.c
  heap_2.c
  event_groups.c
  )

add_library(FreeRTOS STATIC ${LIBFREERTOS_SRCS})
