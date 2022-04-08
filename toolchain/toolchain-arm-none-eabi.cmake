include(CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# Set a toolchain path. You only need to set this if the toolchain isn't in
# your system path. Don't forget a trailing path separator!
set( TC_PATH "" )

# The toolchain prefix for all toolchain executables
set( CROSS_COMPILE arm-none-eabi- )

# specify the cross compiler. We force the compiler so that CMake doesn't
# attempt to build a simple test program as this will fail without us using
# the -nostartfiles option on the command line
set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc)

# We must set the OBJCOPY setting into cache so that it's available to the
# whole project. Otherwise, this does not get set into the CACHE and therefore
# the build doesn't know what the OBJCOPY filepath is
set( CMAKE_OBJCOPY ${TC_PATH}${CROSS_COMPILE}objcopy
    CACHE FILEPATH "The toolchain objcopy command " FORCE )

set( CMAKE_OBJDUMP ${TC_PATH}${CROSS_COMPILE}objdump
    CACHE FILEPATH "The toolchain objdump command " FORCE )

set(COMMON_FLAGS "-fno-common -fno-builtin-printf -ffunction-sections -fdata-sections -fno-strict-aliasing -fmessage-length=0")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=gnu++0x")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections,--undefined=uxTopUsedPriority --specs=nano.specs -nostartfiles")
