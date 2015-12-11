INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)

SET(CMAKE_FIND_ROOT_PATH "/usr/local/arm-none-eabi")
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(COMMON_FLAGS "-fno-builtin -ffunction-sections -fdata-sections -fno-strict-aliasing -fmessage-length=0")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=gnu++0x")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections --specs=nosys.specs -nostdlib ")
