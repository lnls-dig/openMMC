##Include selected board files
message(STATUS "Selected Board: ${BOARD}")
if(NOT BOARD)
  message(FATAL_ERROR "${BoldRed}Target board was not selected! You must pass the flag -DBOARD=<board_name>${ColourReset}")
endif()

if(${BOARD} MATCHES "^(afc-bpm)$")

  if(NOT VERSION)
    message(FATAL_ERROR "${BoldRed}AFC board version was not selected! You must pass the flag -DVERSION=<major>.<minor>${ColourReset}")
  endif()

  message(STATUS "Selected Board version: ${VERSION}")
  if(${VERSION} MATCHES "(3.0)")
    add_subdirectory(afc-bpm/v3_0)
  elseif(${VERSION} MATCHES "(3.1)")
    add_subdirectory(afc-bpm/v3_1)
  endif()

elseif(${BOARD} MATCHES "^(afc-timing)$" )
  add_subdirectory(afc-timing)

elseif(${BOARD} MATCHES "^(sayma)$" )
  add_subdirectory(sayma)

elseif(${BOARD} MATCHES "^(afcz)$" )
  add_subdirectory(afcz)

else()
  message(FATAL_ERROR "${BoldRed}Selected board \"${BOARD}\" is not implemented!${ColourReset}")
endif()

if(${BOARD} MATCHES "^(afc-)")
  add_subdirectory(afc-common)
endif()

#RTM Board
if(BOARD_RTM AND (";${TARGET_MODULES};" MATCHES ";RTM;"))
  message(STATUS "Selected RTM Board support: ${BOARD_RTM}")
  if(${BOARD_RTM} MATCHES "(8sfp)")
    add_subdirectory(rtm-8sfp)
  elseif(${BOARD_RTM} MATCHES "(sayma)")
  	add_subdirectory(rtm-sayma)
  elseif(${BOARD_RTM} MATCHES "(4sfp-3qsfp)")
	add_subdirectory(rtm-4sfp-3qsfp)
  else()
    message(FATAL_ERROR "${BoldRed}Selected RTM board \"${BOARD_RTM}\" is not implemented!${ColourReset}")
  endif()
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
