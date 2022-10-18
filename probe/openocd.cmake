##Program command
if(NOT DEBUG_PROBE)
  set(DEBUG_PROBE "openocd")
endif()

string(TOUPPER ${DEBUG_PROBE} DEBUG_PROBE_UPPERCASE)

if(${DEBUG_PROBE_UPPERCASE} MATCHES "^(OPENOCD-STLINK-V2)$")
    set(PROBE_CONFIG "openocd_stlink-v2.cfg")
elseif(${DEBUG_PROBE_UPPERCASE} MATCHES "^(OPENOCD-OLIMEX)$")
    set(PROBE_CONFIG "openocd_olimex.cfg")
else()
  message(FATAL_ERROR "${ColourBold}${Red}${DEBUG_PROBE} not implemented!${ColourReset}")
endif()

message(STATUS "Selected debug probe: ${DEBUG_PROBE}")


find_program(OPENOCD_BIN openocd)
if(NOT OPENOCD_BIN)
  message(WARNING "${ColourBold}${Red}openocd not found! Can't program the board!${ColourReset}")
endif()

add_custom_target(program_app
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
  DEPENDS ${CMAKE_PROJECT_NAME}
  #Write application firmware
  COMMAND ${OPENOCD_BIN} -f ${PROBE_CONFIG} -s ${CMAKE_SOURCE_DIR}/port/ucontroller/stm32/stm32f30x/ -c exit
  )

add_custom_target(debug_app
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
  DEPENDS ${CMAKE_PROJECT_NAME}
  #Write application firmware and keep openocd running
  COMMAND ${OPENOCD_BIN} -f ${PROBE_CONFIG} -s ${CMAKE_SOURCE_DIR}/port/ucontroller/stm32/stm32f30x/
  )
