set(TRACE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(PROJ_HDRS ${PROJ_HDRS} ${TRACE_PATH})

set(PROJ_SRCS ${PROJ_SRCS} ${TRACE_PATH}/trcBase.c
  ${TRACE_PATH}/trcHardwarePort.c
  ${TRACE_PATH}/trcKernel.c
  ${TRACE_PATH}/trcKernelPortFreeRTOS.c
  ${TRACE_PATH}/trcRecorder.c
  ${TRACE_PATH}/trcUser.c
  )

set(PROJ_SRCS ${PROJ_SRCS} PARENT_SCOPE)
set(PROJ_HDRS ${PROJ_HDRS} PARENT_SCOPE)
