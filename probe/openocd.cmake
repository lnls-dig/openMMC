##Program command

if(NOT DEBUG_PROBE)
  set(DEBUG_PROBE "cmsis-dap")
endif()

if(NOT OPENOCD_TRANSPORT)
  set(OPENOCD_TRANSPORT "swd")
endif()

if(${DEBUG_PROBE} STREQUAL "cmsis-dap")
elseif(${DEBUG_PROBE} STREQUAL "jlink")
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

# Program the chip (available only for LPC1764 so far)
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
