include(CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
cmake_force_c_compiler(arm-none-eabi-gcc GNU)
cmake_force_cxx_compiler(arm-none-eabi-g++ GNU)

set(CMAKE_FIND_ROOT_PATH "/usr/local/arm-none-eabi")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(COMMON_FLAGS "-fno-builtin -ffunction-sections -fdata-sections -fno-strict-aliasing -fmessage-length=0")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=gnu++0x")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections --specs=nosys.specs -nostdlib ")
