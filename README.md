# openMMC
![Travis build status](https://travis-ci.org/lnls-dig/openMMC.svg?branch=master)
Open Source modular IPM Controller firmware

## Installation:
The following packages are needed in your system in order to compile the firmware:
- **gcc-arm-none-eabi**
- **cmake**
- **cmake-gui** (Optional)

**gcc-arm-none-eabi** can be installed from the pre-compiled files found at: https://launchpad.net/gcc-arm-embedded/+download
or you can run the following command under Ubuntu:

    sudo apt-get install gcc-arm-none-eabi

Next step is to clone this repository into your workspace.

	git clone https://github.com/lnls-dig/openMMC

## Compilation

Create a new folder wherever is suitable

	cd <build_folder>

Run CMake using the path to the repository folder as an direct argument and the flag `-DBOARD=<board_name>` and `-DVERSION=<board_version>` to configure the compilation scripts to your specific board hardware

	cmake <path_to_source> -DBOARD=<board_name> -DVERSION=<board_version>

Example:

	cmake ~/openmmc/ -DBOARD=afc -DVERSION=3.1

After creating the build files with CMake, you can compile the firmware using `make`, optionally setting the VERBOSE flag to 1 if you wish to see all the compilation commands

	make [VERBOSE=1]

Both a `.elf` file and a `.bin` file will be generated in the `out` folder. You can use any one you prefer to program your processor.

To clean the compilation files (binaries, objects and dependence files), just run

    make clean

## Programming
After creating the binaries, you can program them to your chip any way you want, using a JTAG/SWD cable, ISP Programmer, custom bootloader, etc.

There are 2 debug probes supported so far: *jlink* and *cmsis-dap*.
In order to select which interface will be used, include the flag `-DDEBUG_PROBE=<probe_name>` when running CMake (this option defaults to `cmsis-dap`).

	cmake ~/openmmc/ -DBOARD=afc -DVERSION=3.1 -DDEBUG_PROBE=cmsis-dap


To transfer only the application firmware, run

    make program_app

To transfer only the bootloader firmware, run

	make program_boot

If you want to erase the whole Flash and copy both firmwares:

	make program_all

### LPCLink
If you own a *LPCLink* or *LPCLink2* board, first make sure it was already flashed with the latest cmsis-dap or jlink firmware.

**NOTE 2**: We only have linker scripts to LPC1764 and LPC1769, so if you wish to compile to a different controller, you'll have to change the `linker/lpc1764_boot.ld` and `linker/lpc1764_app.ld` files, which defines the memory regions, otherwise you'll run into several HardFault errors.
