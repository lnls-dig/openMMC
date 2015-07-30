TOOLCHAIN = arm-none-eabi-
CC = $(TOOLCHAIN)gcc
OBJCOPY = $(TOOLCHAIN)objcopy
PROJ = afcipm
MAKEFILE = Makefile
MAKE = make
MCPU = cortex-m3

#Used for program operation (LPCLink specific software) and newlib files
LPCXPRESSO_PATH=/usr/local/lpcxpresso_7.8.0_426/lpcxpresso

DEFS = -DCORE_M3 -D__CODE_RED -D__USE_LPCOPEN -DNO_BOARD_LIB -D__LPC17XX__ -D__NEWLIB__

LD_SCRIPT = afcipm.ld
MAP = afcipm.map
LD_FLAGS = -T $(LD_SCRIPT) -Xlinker -Map=$(MAP) 
LD_FLAGS += -Xlinker --gc-sections

LPCOPEN_PATH = ./chip
LPCOPEN_SRCPATH = $(LPCOPEN_PATH)/src
LPCOPEN_SRC = $(shell find $(LPCOPEN_SRCPATH) -name '*.c')
LPCOPEN_INCPATH = $(LPCOPEN_PATH)/inc
LPCOPEN_OBJS = $(LPCOPEN_SRC:%.c=%.o)

FREERTOS_PATH = ./FreeRTOS
FREERTOS_SRCPATH = $(FREERTOS_PATH)
FREERTOS_SRC = $(shell find $(FREERTOS_SRCPATH) -name '*.c')
FREERTOS_INCPATH = $(FREERTOS_PATH)/include
FREERTOS_OBJS = $(FREERTOS_SRC:%.c=%.o)

INCLUDES = -I./inc
INCLUDES += -I$(LPCOPEN_INCPATH)
INCLUDES += -I$(FREERTOS_INCPATH)

EXTRA_CFLAGS = -Wall -Os -std=gnu99 -fno-strict-aliasing
CFLAGS = $(DEFS) $(INCLUDES)
CFLAGS += -mcpu=$(MCPU) -mthumb
CFLAGS += -fno-builtin -ffunction-sections -fdata-sections -nostdlib
CFLAGS += $(EXTRA_CFLAGS)

#See if we can find these libraries in a standard path, not depending on LPCXpresso (libgcc.a, libc.a, libm.a, libcr_newlib_nohost.a)
LIB_PATHS = -L$(LPCXPRESSO_PATH)/tools/lib/gcc/arm-none-eabi/4.9.3/thumb
LIB_PATHS += -L/usr/local/lpcxpresso_7.8.0_426/lpcxpresso/tools/arm-none-eabi/lib/thumb

#Do we need this? Dependecy file lists - not being used now
DEPS := -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)"

PROJ_SRCDIR = src
PROJ_SRC = $(shell find $(PROJ_SRCDIR) -name '*.c')
PROJ_OBJS = $(PROJ_SRC:%.c=%.o)

ALL_OBJS = $(PROJ_OBJS)
ALL_OBJS += $(FREERTOS_OBJS)
ALL_OBJS += $(LPCOPEN_OBJS)

all: $(PROJ).bin

%.bin: %.axf
	@echo 'Creating Binary file from .axf'
	$(OBJCOPY) -O binary $< $@

#Linker
%.axf: $(ALL_OBJS) $(MAKEFILE)
	@echo 'Invoking MCU Linker'
	$(CC) $(LIB_PATHS) $(LIBS) $(CFLAGS) $(EXTRA_CFLAGS) $(LD_FLAGS) -o $@ $(ALL_OBJS)
	@echo '$< linked successfully!'

#Sources Compile
%.o: %.c
	@echo 'Building $< '
	$(CC) $(CFLAGS) -o $@ -c $<
	@echo ' $< built successfully!'
	@echo ' '

#Other targets
clean:
	rm -rf $(ALL_OBJS) *.bin *.axf

boot:
	@echo 'Booting LPCLink...'
	$(LPCXPRESSO_PATH)/bin/dfu-util -d 0x471:0xdf55 -c 0 -t 2048 -R -D $(LPCXPRESSO_PATH)/bin/LPCXpressoWIN.enc
	@echo 'LPCLink booted!'

program:
	if [ ! -f $(PROJ).bin ]; then \
	$(MAKE) $(PROJ).bin; \
	fi
	$(MAKE) -i boot
	@echo 'Programing Flash...'
#Program flash and reset chip
	$(LPCXPRESSO_PATH)/bin/crt_emu_cm3_nxp -wire=winusb -pLPC1764 -flash-load-exec=$(PROJ).bin
	@echo 'Programed Successfully!'

.PHONY: all clean boot program
