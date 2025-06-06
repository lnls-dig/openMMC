# openMMC

[![Continuous Integration Status](https://github.com/lnls-dig/openMMC/actions/workflows/build.yml/badge.svg)](https://github.com/lnls-dig/openMMC/actions)

Open Source modular IPM Controller firmware

Documentation: https://lnls-dig.github.io/openMMC

## Table of Contents
- [openMMC](#openmmc)
   * [Installation:](#installation)
   * [Compilation](#compilation)
   * [Programming](#programming)
      + [OpenOCD](#openocd)
      + [LPC-Link1](#lpc-link1)
      + [HPM-Downloader](#hpm-downloader)
      + [ipmitool](#ipmitool)
      + [nxpprog](#nxpprog)
   * [Debugging](#debugging)
   * [IPMI Custom Commands](#ipmi-custom-commands)
      + [Get free heap memory](#get-free-heap-memory)
      + [Commit Hash read](#commit-hash-read)
      + [Clock switch configuration](#clock-switch-configuration)

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

Run CMake using the path to the repository folder as an direct argument and the flag `-DBOARD=<board_name>`(at this moment, only `afc` can be selected) ,`-DVERSION=<board_version>` (`3.1` or `4.0`) and `-DBOARD_RTM=<rtm_name>` (`8sfp` or `lamp`) to configure the compilation scripts to your specific board hardware.

	cmake <path_to_source> -DBOARD=<board_name> -DVERSION=<board_version> -DBOARD_RTM=<rtm_name>

Example:

	cmake ~/openmmc/ -DBOARD=afc -DVERSION=3.1 -DBOARD_RTM=8sfp

After creating the build files with CMake, you can compile the firmware using `make`, optionally setting the VERBOSE flag to 1 if you wish to see all the compilation commands

	make [VERBOSE=1]

Both a `.elf` file and a `.bin` file will be generated in the `out` folder. You can use any one you prefer to program your processor.

To clean the compilation files (binaries, objects and dependence files), just run

    make clean

To make a debug build (to include symbols into elf file, turn off optimizations, etc.) add `-DCMAKE_BUILD_TYPE=Debug` option to `cmake` command. Example:

	cmake ~/openmmc/ -DBOARD=afc -DVERSION=3.1 -DCMAKE_BUILD_TYPE=Debug

## Programming

### OpenOCD
Flashing the MMC microcontroller via SWD/JTAG is supported for CMSIS-DAP and Jlink compatible probes through OpenOCD. You can specify the debug probe with the flag `-DDEBUG_PROBE=<probe_name>`, valid options are `cmsis-dap` (default), `jlink`, `digilent_jtag_hs3` and `xvc` (support is not merged to the official OpenOCD source yet, so you build our patched version: https://github.com/lnls-dig/openocd/tree/fix-afcv3-flashing).

	cmake ~/openmmc/ -DBOARD=afc -DVERSION=3.1 -DDEBUG_PROBE=cmsis-dap

This will create a `openocd.cfg` file in `<build_dir>/out`.

For AFCv4.0 cards, you can program the MMC microcontroller via Xilinx XVC protocol if your MicroTCA crate supports it using the backplane JTAG interface. First, make sure the FPGA is powered off (M1 state), then configure the XVC hostname and port:

To configure a build using XVC:

    cmake ~/openmmc/ -DBOARD=afc -DVERSION=4.0 -DDEBUG_PROBE=xvc -DXVC_HOST=xvc_hostname -DXVC_PORT=xvc_port_number

`Hint:` If the FPGA is powered off and OpenMMC is unresponsive — in the M7 (Error) state — this XVC configuration may still be valid and usable for programming the MMC. In such cases, attempting XVC programming can help recover the board without requiring physical access.

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
> [!WARNING]
> **Disclaimer:** Due to [f06f69f](https://github.com/lnls-dig/openMMC/commit/f06f69f978c11bb8e1a2b12e4846e4bd51f757e4), this alternative is deprecated and ipmitool should be used instead, except when updating the OpenMMC application from older versions.

Another option to program the MMC microcontroler is through [HPM-Downloader](https://github.com/lnls-dig/hpm-downloader).
First, download and compile the HPM. In it's root directory, in order to load the firmware, run

	./bin/hpm-downloader --no-retries --ip <MCH_IP_address> --slot <slots_to_be_updated> --component 1 <path_to_openMMC.bin>

Due to timeout on sending packets, it may fails sometimes, with the Completion Code `0xc3` in the `ACTIVATE_FIRMWARE_UPLOAD` process. If it occurs, just try the same command again, until you receive the `Upgrade success` message.
Now, you have to upload the bootloader, since the old bootloader is incompatible. To succeed this, run

	 ./bin/hpm-downloader --ignore-component-check --ip <MCH_IP_address> --slot <slots_to_be_updated> --component 0 <path_to_newboot.bin>

It's also important to mention that you can use the `--help` command in case of doubt about how to use the HPM commands. Run

	./bin/hpm-downloader --help

### ipmitool
> [!WARNING]
> **Disclaimer:** Due to [e88b30b](https://github.com/lnls-dig/openMMC/tree/e88b30b26c9dc05662d84e35b51fc81d7ecf7561), the ipmitool may fail when trying to upgrade the firmware from v1.5.0. You can use a modified ipmitool utility as a workaround, setting the variable [`max_rq_size`](https://codeberg.org/IPMITool/ipmitool/src/commit/137aeb64cbb493d61d6945cac156aba5f0510780/lib/ipmi_hpmfwupg.c#L1143) to 20.

After [5631857](https://github.com/lnls-dig/openMMC/commit/563185791c8b51ea026680c98ec0ea9587ea645b), it's possible to program the firmware and the bootloader through [ipmitool](https://codeberg.org/IPMITool/ipmitool), for previous releases, you still need to use [hpm-downloader](https://github.com/lnls-dig/hpm-downloader). In order to use it, you have to install the ipmitool, and then generate .hpm files from `OpenMMC.bin` and `newboot.bin`. To generate `.hpm` files, you will need to use [bin2hpm](https://github.com/MicroTCA-Tech-Lab/bin2hpm). If you have bin2hpm in your `$PATH`, the `.hpm` files will be automatically generated for you, provided you build from [0095b14](https://github.com/lnls-dig/openMMC/commit/0095b14667afe844113725228671d8810b45d9e0) or more recent versions.
After generate the files, you can use the following commands to program the MMC microcontroller.
To upgrade the firmware, use

    ipmitool -I lan -H host_name_mch -A none -T 0x82 -m 0x20 -t (112 + num_slot*2 in hexadecimal) hpm upgrade openMMC.hpm activate

To upgrade the bootloader, use

    ipmitool -I lan -H host_name_mch -A none -T 0x82 -m 0x20 -t (112 + num_slot*2 in hexadecimal) hpm upgrade newboot.hpm activate

### nxpprog
> [!WARNING]
> **Disclaimer:** Only supported in AFCv4.0.2

In AFCv4, it's possible to program the firmware and bootloader via serial port using [nxpprog](https://github.com/lnls-dig/nxpprog). In order to use it, install nxpprog and then execute the following commands:
To upgrade the application, use

    ./nxpprog.py  <serial_device> <path_to_openMMC.bin> --addr 0x20000 --control

To upgrade the bootloader, use

    ./nxpprog.py  <serial_device> <path_to_newboot.bin> --control

## Debugging
It is possible to debug the MMC firmware using OpenOCD and GDB. First, connect OpenOCD with the debug probe using the `out/openocd.cfg` file generated by cmake in the build directory:

	openocd -f out/openocd.cfg

Then open GDB:

	$ arm-none-eabi-gdb out/openMMC.elf
	(gdb) target remote localhost:3333

Now you can use the typical GDB commands to inspect the program flow and variables. Some useful commands when interacting with a microcontroller trough OpenOCD are listed below:

	(gdb) monitor reset halt # Resets the microcontroller and immediately halts
	(gdb) monitor reset run  # Resets the microcontroller and starts executing
	(gdb) load               # Reload the firmware into flash


## IPMI Custom Commands
The IPMI allow us to create custom commands according to the project needs. [ipmitool](https://codeberg.org/IPMITool/ipmitool) can be used to send the commands

### Get free heap memory
It is possible to monitor the MMC's memory usage via IPMI. To obtain the total free heap use command 0x01, netfn_id 0x32. The returned data will be the number of free heap bytes, encoded as a 32 bits unsigned integer, little-endian.

    ipmitool -I lan -H mch_host_name -A none -T 0x82 -m 0x20 -t (112 + num_slot*2) raw 0x32 0x01

### Commit Hash read
After loading a new firmware, you can use an ipmi command to read its commit hash to ensure that the running version is the one you want.
The command is the above:


    ipmitool -I lan -H mch_host_name -A none -T 0x82 -m 0x20 -t (112 + num_slot*2) raw 0x32 0x02

### Clock switch configuration
> [!IMPORTANT]
> After [7a919489](https://github.com/lnls-dig/openMMC/tree/7a91948975f87f6613b28fc1ea562a2a7bb3c475) the clock switch configuration is no longer hardcoded for each board. Instead, it must be configured through an IPMI command.

For the AFC v3.1 (ADN4604ASVZ) you can use the [script](/scripts/afcv3.1-clksw-config.py) for generate the configuration, and use the json configuration for [AFCv3.1-BPM](/config/lnls/afcv3.1-bpm-clk-cfg.json) or [AFCv3.1-Timing](/config/lnls/afcv3.1-timing-clk-cfg.json). If you want to generate a new configuration, use the following scheme:
- **Port I/O (bit 7)**: Use it to configure the port as an input ('0') or output ('1'). Unused ports should be left configured as inputs;
- **Output Port Signal Source (bits 0 to 3)**: Select the input port for the respective output port.

For the AFC v4 (IDT 8V54816) you can use the following scheme:
For the AFC v4.0 (IDT 8V54816) you can use the [script](/scripts/afcv4-clksw-config.py) for generate the configuration, and use the json configuration for [AFCv4.0-FOFB](/config/lnls/afcv4-fofb-clk-cfg.json). If you want to generate a new configuration, use the following scheme:
- **Port I/O (bit 7)**: Use it to configure the port as an input ('0') or output ('1'). Unused ports should be left configured as inputs;
- **Termination On/Off (bit 6)**: Use to set the internal termination. '0' is off (high-impedance), '1' is on (100 $\Omega$)
- **Polarity (bit 5)**: Set the channel polarity. '0' for inverted, '1' for non-inverted
- **Output Port Signal Source (bits 0 to 3)**: Select the input port for the respective output port.


The command to write the configuration is the above:

    ipmitool -I lan -H mch_host_name -A none -T 0x82 -m 0x20 -t (112 + num_slot*2) raw 0x32 0x03 <configuration_array_in_hex>

To read the actual configuration, use:

    ipmitool -I lan -H mch_host_name -A none -T 0x82 -m 0x20 -t (112 + num_slot*2) raw 0x32 0x04
