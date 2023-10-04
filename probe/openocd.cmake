##Program command

function(check_probe_transport transport_list selected_transport)
  set(transport_available FALSE)
  foreach(transport IN LISTS transport_list)
    if(${selected_transport} STREQUAL ${transport})
      set(transport_available TRUE)
    endif()
  endforeach()
  if(NOT transport_available)
    message(FATAL_ERROR "Debug probe does not support transport type '${selected_transport}'")
  endif()
endfunction()

if(NOT DEBUG_PROBE)
  message(NOTICE "No debug probe selected! Targets program_boot, program_app and program_all will not be generated!")
else()

  if(NOT OPENOCD_TRANSPORT)
    message(NOTICE "No debug probe transport set, 'jtag' selected by default.")
    set(OPENOCD_TRANSPORT "jtag")
  endif()

  # Selects the OpenOCD debug probe file
  if(${DEBUG_PROBE} STREQUAL "digilent_jtag_hs3")
    set(OPENOCD_DEBUG_PROBE_CMD "source [find interface/ftdi/digilent_jtag_hs3.cfg]")
    check_probe_transport("jtag" ${OPENOCD_TRANSPORT})
  elseif(${DEBUG_PROBE} STREQUAL "cmsis-dap")
    set(OPENOCD_DEBUG_PROBE_CMD "source [find interface/cmsis-dap.cfg]")
    check_probe_transport("jtag;swd" ${OPENOCD_TRANSPORT})
  elseif(${DEBUG_PROBE} STREQUAL "jlink")
    set(OPENOCD_DEBUG_PROBE_CMD "source [find interface/jlink.cfg]")
    check_probe_transport("jtag;swd" ${OPENOCD_TRANSPORT})
  elseif(${DEBUG_PROBE} STREQUAL "xvc")
    if(NOT XVC_HOST)
      message(FATAL_ERROR "No xvc host selected, use -DXVC_HOST=<hostname> to select it.")
    elseif(NOT XVC_PORT)
      message(FATAL_ERROR "No xvc port selected, use -DXVC_PORT=<port> to select it.")
    else()
      set(OPENOCD_DEBUG_PROBE_CMD "adapter driver xvc\nxvc_host ${XVC_HOST}\nxvc_port ${XVC_PORT}\nreset_config none")
      check_probe_transport("jtag" ${OPENOCD_TRANSPORT})
    endif()
  else()
    message(FATAL_ERROR "${DEBUG_PROBE} not supported!")
  endif()

  message(STATUS "Selected debug probe: ${DEBUG_PROBE}")

  if(${TARGET_CONTROLLER} MATCHES "^([lL][pP][cC]17[0-9][0-9])")
    set(OPENOCD_TARGET lpc17xx)
  else()
    message(FATAL_ERROR "Target ${TARGET_CONTROLLER} not supported")
  endif()

  configure_file(probe/openocd.cfg.in ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/openocd.cfg)

  # FIXME: The program_* targets hardcodes the program memory
  # addresses for the application and bootloader. This might be
  # different for other microcontrollers. Ideally, OpenOCD should use
  # the ELF files to program those, but from my tests, it doesn't work
  # because OpenOCD erases all flash before writing the binary from
  # the ELF file.
  add_custom_target(program_boot
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
    DEPENDS newboot
    #Write bootloader firmware
    COMMAND openocd -f openocd.cfg -c "init" -c "program newboot.bin 0x0000 verify" -c "reset run" -c "shutdown"
  )

  add_custom_target(program_app
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
    DEPENDS ${CMAKE_PROJECT_NAME}
    #Write application firmware
    COMMAND openocd -f openocd.cfg -c "init" -c "program ${CMAKE_PROJECT_NAME}.bin 0x2000 verify" -c "reset run" -c "shutdown"
  )

  add_custom_target(program_all
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
    DEPENDS ${CMAKE_PROJECT_NAME} newboot
    #Erase all flash
    COMMAND openocd -f openocd.cfg -c "init" -c "halt" -c "flash erase_sector 0 0 last" -c "flash erase_check 0" -c "shutdown"
    #Write application and bootloader firmwares
    COMMAND make -C ${CMAKE_BINARY_DIR} program_boot
    COMMAND make -C ${CMAKE_BINARY_DIR} program_app
  )
endif()

add_custom_target(full_binary
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
  DEPENDS ${CMAKE_PROJECT_NAME} newboot
  #Create a pad file with the total size of the bootloader (0x2000)
  COMMAND dd if=/dev/zero bs=1 count=8192 | tr \"\\000\" \"\\377\" > newboot_pad.bin
  #Add padding bytes to the bootloader binary
  COMMAND dd if=newboot.bin of=newboot_pad.bin conv=notrunc
  #Concatenate bootloader and app binaries
  COMMAND cat newboot_pad.bin ${CMAKE_PROJECT_NAME}.bin > openMMC_full.bin
  )
