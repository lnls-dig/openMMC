# openMMC

[![Continuous Integration Status](https://github.com/lnls-dig/openMMC/actions/workflows/build.yml/badge.svg)](https://github.com/lnls-dig/openMMC/actions)

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

### OpenOCD
Flashing the MMC microcontroller via SWD/JTAG is supported for CMSIS-DAP and Jlink compatible probes through OpenOCD. You can specify the debug probe with the flag `-DDEBUG_PROBE=<probe_name>`, valid options are `cmsis-dap` (default) and `jlink`.

	cmake ~/openmmc/ -DBOARD=afc -DVERSION=3.1 -DDEBUG_PROBE=cmsis-dap

This will create a `openocd.cfg` file in `<build_dir>/out`.

To flash the application firmware only, run

    make program_app

To flash the bootloader firmware only, run

	make program_boot

If you want to erase the whole Flash and flash both firmwares:

	make program_all

### LPC-Link1
It is possible to flash the MMC microcontroller with the LPC-Link1 adapter by using the `probe/lpclink1-flash.sh` script. You will need to have MCUXpresso IDE installed in your computer. This script assumes that the MCUXpresso binaries are located in `/usr/local/mcuxpressoide/ide/binaries/` by default, you can change this path by setting the `MCUXPRESSOIDE_BIN` environment variable.

	./probe/lpclink1-flash.sh firmware.bin LPC1764 0x0000

### HPM-Downloader
> :warning: **Disclaimer:** Due to [f06f69f](https://github.com/lnls-dig/openMMC/commit/f06f69f978c11bb8e1a2b12e4846e4bd51f757e4), this alternative is deprecated and ipmitool should be used instead, except when updating the OpenMMC application from older versions.

Another option to program the MMC microcontroler is through [HPM-Downloader](https://github.com/lnls-dig/hpm-downloader).
First, download and compile the HPM. In it's root directory, in order to load the firmware, run

	./bin/hpm-downloader --no-retries --ip <MCH_IP_address> --slot <slots_to_be_updated> --component 1 <path_to_openMMC.bin>

Due to timeout on sending packets, it may fails sometimes, with the Completion Code `0xc3` in the `ACTIVATE_FIRMWARE_UPLOAD` process. If it occurs, just try the same command again, until you receive the `Upgrade success` message.
Now, you have to upload the bootloader, since the old bootloader is incompatible. To succeed this, run

	 ./bin/hpm-downloader --ignore-component-check --ip <MCH_IP_address> --slot <slots_to_be_updated> --component 0 <path_to_newboot.bin>

It's also important to mention that you can use the `--help` command in case of doubt about how to use the HPM commands. Run

	./bin/hpm-downloader --help

### ipmitool
After [5631857](https://github.com/lnls-dig/openMMC/commit/563185791c8b51ea026680c98ec0ea9587ea645b), it's possible to program the firmware and the bootloader through [ipmitool](https://codeberg.org/IPMITool/ipmitool), for previous releases, you still need to use [hpm-downloader](https://github.com/lnls-dig/hpm-downloader). In order to use it, you have to install the ipmitool, and then generate .hpm files from `OpenMMC.bin` and `newboot.bin`. To generate `.hpm` files, you will need to use [bin2hpm](https://github.com/MicroTCA-Tech-Lab/bin2hpm). If you have bin2hpm in your `$PATH`, the `.hpm` files will be automatically generated for you, provided you build from [0095b14](https://github.com/lnls-dig/openMMC/commit/0095b14667afe844113725228671d8810b45d9e0) or more recent versions.
After generate the files, you can use the following commands to program the MMC microcontroller.
To upgrade the firmware, use

    ipmitool -I lan -H host_name_mch -A none -T 0x82 -m 0x20 -t (112 + num_slot*2 in hexadecimal) hpm upgrade openMMC.hpm

To upgrade the bootloader, use

    ipmitool -I lan -H host_name_mch -A none -T 0x82 -m 0x20 -t (112 + num_slot*2 in hexadecimal) hpm upgrade newboot.hpm

## Debugging
It is possible to debug the MMC firmware using OpenOCD and GDB. First, connect OpenOCD with the debug probe using the `out/openocd.cfg` file generated by cmake in the build directory:

	openocd -f out/openocd.cfg

Then open GDB:

	$ arm-none-eabi-gdb out/openMMC.axf
	(gdb) target remote localhost:3333

Now you can use the typical GDB commands to inspect the program flow and variables. Some useful commands when interacting with a microcontroller trough OpenOCD are listed below:

	(gdb) monitor reset halt # Resets the microcontroller and immediately halts
	(gdb) monitor reset run  # Resets the microcontroller and starts executing
	(gdb) load               # Reload the firmware into flash
