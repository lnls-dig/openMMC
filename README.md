# openMMC
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

Go to the repository folder

	cd /path/to/repo/

Run CMake passing the flag `-DBOARD=<board_name>` to configure the compilation scripts to your specific board hardware

	cmake -DBOARD=<board_name>

You can set your board options in the file `build_cfg/config.cmake`. You can change the Controller, Board name, version and select which modules you want to be included in your compilation.

After changing the desired options, run the CMake configuration command again (the CMakeCache file will not be edited by this command) and compile the firmware:

	make -s

*NOTE: The compiler will return several warnings, most of them are regarding the LPCOpen libraries and do not affect the library functionality.*

Both a `.axf` file and a `.bin` file will be generated in the `out` folder. You can use any one you prefer to program your processor.

To clean the compilation files (binaries, objects and dependence files), just run

    make clean

## Programming LPC1764
If you own a *LPCLink* board, you can use it to program the LPC1764 processor via its JTAG interface

First, make sure that the LPCXpresso path is set correctly inside `<openMMC_root_folder>/CMakeLists.exe`

	set(LPCXPRESSO_PATH <lpcxpresso_path>)

*NOTE*: You can also use cmake-gui to set this option.

To transfer only the application firmware, run

    make program_app

To transfer only the bootloader firmware, run

	make program_boot

If you want to erase the whole Flash and copy both firmwares:

	make program_all

**NOTE**: In this case you must have the LPCXpresso installed in your machine, since we need to use some initialization scripts that they provide.

**NOTE 2**: We only have linker scripts to LPC1764, so if you wish to compile to a different target, you'll have to change the `linker/lpc1764_boot.ld` and `linker/lpc1764_app.ld` files, which defines the memory regions, otherwise you'll run into some HardFault errors.
