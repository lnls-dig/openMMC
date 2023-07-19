message(STATUS "Selected Controller: ${TARGET_CONTROLLER}")

if(NOT TARGET_CONTROLLER)
  message(FATAL_ERROR "${BoldRed}Target controller was not selected! You can pass the flag -DTARGET_CONTROLLER=<controller> or set it on the board CMakeLists.txt${ColourReset}")
endif()

if(${TARGET_CONTROLLER} MATCHES "^(lpc|LPC)17")
  add_subdirectory(nxp/lpc17xx)

else ()
  message(FATAL_ERROR "${BoldRed}Selected Controller \"${TARGET_CONTROLLER}\" is not implemented!${ColourReset}")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" PARENT_SCOPE)
set(UCONTROLLER_SRCS ${UCONTROLLER_SRCS} PARENT_SCOPE)
set(UCONTROLLER_HDRS ${UCONTROLLER_HDRS} PARENT_SCOPE)
set(UCONTROLLER_APP_LD_SCRIPT ${UCONTROLLER_APP_LD_SCRIPT} PARENT_SCOPE)
