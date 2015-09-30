TOOLCHAIN = arm-none-eabi-
CC = $(TOOLCHAIN)gcc
AR = $(TOOLCHAIN)ar
OBJCOPY = $(TOOLCHAIN)objcopy
PROJ = afcipm
MAKEFILE = Makefile
MAKE = make
MCPU = cortex-m3

LIBDIR = lib
BUILDDIR = out

#Used for program operation (LPCLink specific software)
LPCXPRESSO_PATH=/usr/local/lpcxpresso_7.8.0_426/lpcxpresso

#Flags to be passed on to gcc
DEFS = -DDEBUG -DCORE_M3 -D__CODE_RED -D__USE_LPCOPEN -DNO_BOARD_LIB -D__LPC17XX__ -D__NEWLIB__

LD_SCRIPT = linker/lpc1764.ld
MAP = linker/afcipm.map
LD_FLAGS = -T $(LD_SCRIPT) -Xlinker -Map=$(MAP)
LD_FLAGS += -Xlinker --gc-sections
LD_FLAGS += -mcpu=$(MCPU) -mthumb
LD_FLAGS += --specs=nosys.specs

LPCOPEN_LIBNAME = lpcopen
LPCOPEN_LIBFILE = $(LIBDIR)/lib$(LPCOPEN_LIBNAME).a
LPCOPEN_PATH = ./port/nxp/lpc17xx/lpcopen
LPCOPEN_SRCPATH = $(LPCOPEN_PATH)/src
LPCOPEN_SRC = $(shell find $(LPCOPEN_SRCPATH) -name '*.c')
LPCOPEN_INCPATH = $(LPCOPEN_PATH)/inc
LPCOPEN_OBJS = $(LPCOPEN_SRC:%.c=%.o)

FREERTOS_LIBNAME = freertos
FREERTOS_LIBFILE = $(LIBDIR)/lib$(FREERTOS_LIBNAME).a
FREERTOS_PATH = ./FreeRTOS
FREERTOS_SRCPATH = $(FREERTOS_PATH)
FREERTOS_SRC = $(shell find $(FREERTOS_SRCPATH) -name '*.c')
FREERTOS_INCPATH = $(FREERTOS_PATH)/include
FREERTOS_OBJS = $(FREERTOS_SRC:%.c=%.o)

INCLUDES = -I./
INCLUDES += -I./board/afcv3
INCLUDES += -I./port/nxp/lpc17xx
INCLUDES += -I./modules
INCLUDES += -I./modules/sensors
INCLUDES += -I$(LPCOPEN_INCPATH)
INCLUDES += -I$(FREERTOS_INCPATH)

EXTRA_CFLAGS = -Wall -O0 -g3 -std=gnu99
CFLAGS = $(DEFS) $(INCLUDES)
CFLAGS += -mcpu=$(MCPU) -mthumb
CFLAGS += -fno-builtin -ffunction-sections -fdata-sections -fno-strict-aliasing  -fmessage-length=0 -nostdlib
CFLAGS += $(EXTRA_CFLAGS)

LIBS += -lgcc -lc -lm -l$(FREERTOS_LIBNAME) -l$(LPCOPEN_LIBNAME)

DEPS = -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)"

PROJ_SRCDIR = ./
PROJ_SRC = $(shell find $(PROJ_SRCDIR) -type d \( -path $(FREERTOS_PATH) -o -path $(LPCOPEN_PATH) \) -prune -o -name '*.c' -print)
PROJ_OBJS = $(PROJ_SRC:%.c=%.o)

.PRECIOUS: %.axf %.bin

all: $(PROJ).bin

folders:
	@mkdir -p $(LIBDIR)
	@mkdir -p $(BUILDDIR)

%.bin: $(BUILDDIR)/%.axf
	@echo 'Creating Binary file from .axf'
	$(OBJCOPY) -O binary $< $(BUILDDIR)/$@
	@echo 'Binary file created succesfully!'
	@echo ' '

#Linker
%.axf: folders $(FREERTOS_LIBFILE) $(LPCOPEN_LIBFILE) $(PROJ_OBJS)
	@echo 'Invoking MCU Linker'
	$(CC) $(LD_FLAGS) -o $(BUILDDIR)/$(notdir $@) $(PROJ_OBJS) -L$(LIBDIR) $(LIBS)
	@echo '$@ linked successfully!'
	@echo ' '

#Sources Compile
%.o: %.c
	@echo 'Building $< '
	$(CC) $(CFLAGS) $(DEPS) -o $@ -c $<
-include $(@:%.o=%.d)
	@echo ' $< built successfully!'
	@echo ' '

#Archiver for FreeRTOS objects
$(FREERTOS_LIBFILE): $(FREERTOS_OBJS)
	@echo 'Archiving $@ objs'
	$(AR) -r $@ $(FREERTOS_OBJS)
	@echo 'Library $@ successfully created'
	@echo ' '

#Archiver for LPCOpen objects
$(LPCOPEN_LIBFILE): $(LPCOPEN_OBJS)
	@echo 'Archiving $@ objs'
	$(AR) -r $@ $(LPCOPEN_OBJS)
	@echo 'Library $@ successfully created'
	@echo ' '

#Other targets
clean:
	@rm -rf $(PROJ_OBJS) $(PROJ_OBJS:%.o=%.d) *.map
	@rm -rf $(BUILDDIR)

mrproper: clean
	@rm -rf $(LPCOPEN_OBJS) $(LPCOPEN_OBJS:%.o=%.d)
	@rm -rf $(FREERTOS_OBJS) $(FREERTOS_OBJS:%.o=%.d)
	@rm -rf $(LIBDIR)

boot:
	@echo 'Booting LPCLink...'
	$(LPCXPRESSO_PATH)/bin/dfu-util -d 0x471:0xdf55 -c 0 -t 2048 -R -D $(LPCXPRESSO_PATH)/bin/LPCXpressoWIN.enc
	@echo 'LPCLink booted!'
	@echo ' '

program:
	@if [ ! -f $(BUILDDIR)/$(PROJ).axf ]; then \
	$(MAKE) $(PROJ).axf; \
	fi
	@$(MAKE) -i boot
	@echo 'Programing Flash...'
#Program flash and reset chip
	$(LPCXPRESSO_PATH)/bin/crt_emu_cm3_nxp -wire=winusb -pLPC1764 -flash-load-exec=$(BUILDDIR)/$(PROJ).axf
	@echo 'Programed Successfully!'
	@echo ' '

#Debug print
print-%: ; @echo $*=$($*)

.PHONY: all clean mrproper boot program folders
