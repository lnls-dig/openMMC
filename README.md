# AFCIPM
MMC firmware for AFC boards

## Installation:
To compile this firmware, it is necessary to install **`GCC ARM Embedded`**, a specific toolchain to Cortex-M and Cortex-R processors.
You can download it's binary and install yourself:
* https://launchpad.net/gcc-arm-embedded/+download

or run the following command:

    sudo apt-get install gcc-arm-none-eabi

## Compilation

Go to the repository folder

    cd afcipm

Compile the firmware

    make all

It will create a `.axf` file and a `.bin` file, which you can use to program your processor.

If you want to create only a `.bin` file, or specify a different output name, run

    make <output_name>.bin

To clean the compilation files (binaries, objects and dependence files), just run

    make clean

## Programming LPC1764
If you own a *LPCLink* board, you can use it to program the LPC1764 processor via its JTAG interface

    make program

**NOTE**: In this case you must have the LPCXpresso installed in your machine, since we need to use some initialization scripts that they provide.
**NOTE 2**: We only have linker scripts to LPC1764, so if you wish to compile to a different target, you'll have to change the `afcipm_mem.ld` file, which defines the memory regions, otherwise you'll run into some HardFault errors.
