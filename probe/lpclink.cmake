##Program command
if(NOT DEBUG_PROBE)
  set(DEBUG_PROBE "LPCLink")
endif()

string(TOUPPER ${DEBUG_PROBE} DEBUG_PROBE_UPPERCASE)

if(${DEBUG_PROBE_UPPERCASE} MATCHES "^(LPCLINK)$")
  set(DFU_DEVICE "0x0471:0xDF55")
  set(DFU_FIRMWARE "LPCXpressoWIN.enc")
  set(NXP_PROG_NAME "crt_emu_cm3_nxp")
  set(NXP_PROG_FLAGS "-wire=winusb")
elseif(${DEBUG_PROBE_UPPERCASE} MATCHES "^(LPCLINK2)$")
  set(DFU_DEVICE "0x1FC9:0x000C")
  set(NXP_PROG_NAME "crt_emu_cm_redlink")
  set(DFU_FIRMWARE "LPC432x_CMSIS_DAP_V5_173.bin.hdr")
  set(NXP_PROG_FLAGS "")
else()
  message(FATAL_ERROR "${DEBUG_PROBE} not implemented!")
endif()

message(STATUS "Selected debug probe: ${DEBUG_PROBE}")

set(LPCXPRESSO_INSTALL_PATH /usr/local/lpcxpresso/lpcxpresso/bin)

find_program(DFU_UTIL dfu-util)
if(NOT DFU_UTIL)
  message(WARNING "${ColourBold}${Red}Dfu-util not found! Can't boot the programmer board!${ColourReset}")
endif()

find_program(NXP_PROG ${NXP_PROG_NAME} PATHS ${LPCXPRESSO_INSTALL_PATH})
if(NOT NXP_PROG)
  message(WARNING "${ColourBold}${Red}${NXP_PROG_NAME} not found! Can't program the controller!${ColourReset}")
else()
  get_filename_component( DFU_FIRMWARE_PATH ${NXP_PROG} DIRECTORY )
endif()

# Program the chip (available only for LPC1764 so far)
add_custom_target(program_boot
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
  DEPENDS bootloader
  #Boot the programmer
  COMMAND ${DFU_UTIL} -d ${DFU_DEVICE} -c 0 -t 2048 -R -D ${DFU_FIRMWARE_PATH}/${DFU_FIRMWARE} || true
  #Write bootloader firmware
  COMMAND ${NXP_PROG} -p${TARGET_CONTROLLER} -g ${NXP_PROG_FLAGS} -flash-load=bootloader.bin -load-base=0x0000
  )

add_custom_target(program_app
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
  DEPENDS ${CMAKE_PROJECT_NAME}
  #Boot the programmer
  COMMAND ${DFU_UTIL} -d ${DFU_DEVICE} -c 0 -t 2048 -R -D ${DFU_FIRMWARE_PATH}/${DFU_FIRMWARE} || true
  #Write application firmware
  COMMAND ${NXP_PROG} -p${TARGET_CONTROLLER} -g ${NXP_PROG_FLAGS} -flash-load-exec=${CMAKE_PROJECT_NAME}.bin -load-base=0x2000
  )

add_custom_target(program_all
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
  DEPENDS ${CMAKE_PROJECT_NAME} bootloader
  COMMAND ${DFU_UTIL} -d ${DFU_DEVICE} -c 0 -t 2048 -R -D ${DFU_FIRMWARE_PATH}/${DFU_FIRMWARE} || true
  #Write application and bootloader firmwares
  COMMAND ${NXP_PROG} -p${TARGET_CONTROLLER} -g  ${NXP_PROG_FLAGS} -flash-mass-erase
  COMMAND sleep 0.5
  COMMAND make -C ${CMAKE_BINARY_DIR} program_boot
  COMMAND sleep 0.5
  COMMAND make -C ${CMAKE_BINARY_DIR} program_app
  )

add_custom_target(full_binary
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
  DEPENDS ${CMAKE_PROJECT_NAME} bootloader
  #Create a pad file with the total size of the bootloader (0x2000)
  COMMAND dd if=/dev/zero bs=1 count=8192 | tr \"\\000\" \"\\377\" > bootloader_pad.bin
  #Add padding bytes to the bootloader binary
  COMMAND dd if=bootloader.bin of=bootloader_pad.bin conv=notrunc
  #Concatenate bootloader and app binaries
  COMMAND cat bootloader_pad.bin ${CMAKE_PROJECT_NAME}.bin > openMMC_full.bin

  #Program full_binary (for tests only)
  #COMMAND ${DFU_UTIL} -d ${DFU_DEVICE} -c 0 -t 2048 -R -D ${DFU_FIRMWARE_PATH}/${DFU_FIRMWARE} || true
  #COMMAND ${NXP_PROG} ${NXP_PROG_FLAGS} -flash-load-exec=openMMC_full.bin -load-base=0x0
  )
