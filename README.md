# openMMC

[![Travis build status](https://travis-ci.org/lnls-dig/openMMC.svg?branch=master)](https://travis-ci.org/lnls-dig/openMMC)

Open Source modular IPM Controller firmware

Documentation: https://lnls-dig.github.io/openMMC

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

Both a `.axf` file and a `.bin` file will be generated in the `out` folder. You can use any one you prefer to program your processor.

To clean the compilation files (binaries, objects and dependence files), just run

    make clean

To make a debug build (to include symbols into axf file, turn off optimizations, etc.) add `-DCMAKE_BUILD_TYPE=Debug` option to `cmake` command. Example:

	cmake ~/openmmc/ -DBOARD=afc -DVERSION=3.1 -DCMAKE_BUILD_TYPE=Debug

## Programming
After creating the binaries, you can program them to your chip any way you want, using a JTAG cable, ISP Programmer, custom bootloader, etc.
There are 2 program interfaces supported so far: *LPCLink* and *LPCLink2*
In order to select which interface will be used, include the flag `-DDEBUG_PROBE=<probe_name>` when running CMake (this option defaults to `LPCLink`).

	cmake ~/openmmc/ -DBOARD=afc -DVERSION=3.1 -DDEBUG_PROBE=LPCLink2


### LPCLink
If you own a *LPCLink* or *LPCLink2* board, you can use it to program the LPC1764 processor via its JTAG interface

**NOTE**: In this case you **must** have the LPCXpresso installed in your machine, since we need to use the binaries for the interface chip on the LPCLink that they provide.

The CMake script should be able to find LPCXpresso path, but if this is not possible, open the `<openMMC_root_folder>/CMakeLists.txt` and change the following line

	set(LPCXPRESSO_PATH <lpcxpresso_path>)

*NOTE*: You can also use cmake-gui to set this option.

To transfer only the application firmware, run

    make program_app

To transfer only the bootloader firmware, run

	make program_boot

If you want to erase the whole Flash and copy both firmwares:

	make program_all

**NOTE 2**: We only have linker scripts to LPC1764 and LPC1769, so if you wish to compile to a different controller, you'll have to change the `linker/lpc1764_boot.ld` and `linker/lpc1764_app.ld` files, which defines the memory regions, otherwise you'll run into several HardFault errors.

### FlashMagic

With AFC v4 it is possible to program MMC using only a microUSB cable. As for now, the most reliable option is to use [FlashMagic](https://www.flashmagictool.com/) (**it may be possible to use [nxpprog](https://github.com/Technosystem-Labs/nxpprog), but for now it seems to be less reliable, we're working on it**).

If you are on Windows the procedure is straightforward: 

1. Install and run FlashMagic.
2. Select **LPC1768 (LPC1700)** device.
3. Identify COM port related to the MMC (it will be 4th port of the FTDI that appears in the system after connecting AFC) and select it in *Serial Port:*.
4. In the firmware filed select **openMMC.bin** file that is placed in `<build dir>/out/openMMC.bin`. This scenario currently does not support flashing with bootloader. The only drawback is lack of possibility of firmware update via IPMI.
5. Press big green button.
6. Power cycle board.

If you are on Linux you will need to use Wine. For detailed instruction please refer to [this](http://linuxkernel51.blogspot.com/2017/12/flashmagic-on-ubuntu.html) post.
