##Include selected board files
message(STATUS "Selected Board: ${BOARD}")
if(NOT BOARD)
  message(FATAL_ERROR "${BoldRed}Target board was not selected!${ColourReset}")
endif()

#BUG: Regexp will match AFC and AFCK
if(${BOARD} MATCHES "^(AFC|afc)")
  add_subdirectory(afc)
else()
  message(FATAL_ERROR "${BoldRed}Selected board \"${BOARD}\" is not implemented!${ColourReset}")
endif()

#Normalize strings to upper case
string(TOUPPER "${TARGET_CONTROLLER}" TARGET_CONTROLLER_UPPER)
set(TARGET_CONTROLLER "${TARGET_CONTROLLER_UPPER}" CACHE STRING "Target Controller")
string(TOUPPER "${TARGET_BOARD_NAME}" TARGET_BOARD_UPPER)
set(TARGET_BOARD_NAME "${TARGET_BOARD_UPPER}" CACHE STRING "Target Board Name")

#Set the modified variables in the main scope
set(PROJ_SRCS ${PROJ_SRCS} PARENT_SCOPE)
set(PROJ_HDRS ${PROJ_HDRS} PARENT_SCOPE)
set(TARGET_MODULES ${TARGET_MODULES} PARENT_SCOPE)
set(MODULES_FLAGS ${MODULES_FLAGS} PARENT_SCOPE)
