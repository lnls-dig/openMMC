set(BOOTLOADER_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(BOOTLOADER_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lpc17_clock.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lpc17_iap.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lpc17_pincfg.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lpc17_progmem.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lpc17_uart.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/start_app.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/startup.s
  )

set(BOOTLOADER_SRCS ${BOOTLOADER_SRCS} PARENT_SCOPE)
set(BOOTLOADER_INC_PATH ${BOOTLOADER_INC_PATH} PARENT_SCOPE)

set(COMMON_FLAGS "")
set(CMAKE_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_C_FLAGS_DEBUG "-Og -g3 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-Os")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-Os -g3")

set(CMAKE_ASM_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_ASM_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
set(CMAKE_ASM_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO})

set(LPC17_BOOT_FLAGS "-fno-tree-loop-distribute-patterns -fno-exceptions -fno-unwind-tables -fdata-sections -ffunction-sections -Wall -mcpu=cortex-m3 -mlittle-endian -mthumb -I \"${BOOTLOADER_INC_PATH}\" -mlong-calls")
set(LPC17_BOOT_LINK_FLAGS "${LPC17_BOOT_FLAGS} -Wl,--gc-sections -nostdlib -nostartfiles -T \"${CMAKE_CURRENT_SOURCE_DIR}/../linker/${TARGET_CONTROLLER}_boot.ld\"")
message(STATUS "LPC17_BOOT_FLAGS: ${LPC17_BOOT_FLAGS}")
message(STATUS "LPC17_BOOT_LINK_FLAGS: ${LPC17_BOOT_LINK_FLAGS}")
message(STATUS "LPC17_FLAGS: ${LPC17_FLAGS}")
add_executable(newboot ${BOOTLOADER_SRCS})
set_target_properties(newboot PROPERTIES
  COMPILE_FLAGS ${LPC17_BOOT_FLAGS}
  SUFFIX ".elf"
  LINK_FLAGS ${LPC17_BOOT_LINK_FLAGS}
  )
add_custom_command(TARGET newboot POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary newboot.elf newboot.bin
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  COMMENT "Generate a raw binary image of the bootloader"
  )
