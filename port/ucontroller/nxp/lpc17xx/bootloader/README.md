# LPC17xx bootloader (newboot)

This bootloader is responsible for copying the new firmware image from the firmware update area to the application or bootloader (self-update) area. It shouldn't be confused with the LPC17xx ROM bootloader (firmware programming via serial port).

## Flash layout

The flash memory is divided in three regions defined in the linker script for each target.

LPC1764:
* Bootloader 0x00000000 - 0x00001FFF (8 KiB);
* Application 0x00002000 - 0x000FFFF (56 KiB);
* Firmware update 0x00010000 - 0x0001DFFF (56 KiB);
* New firmware record address: 0x0001DF00.

LPC1768:
* Bootloader 0x00000000 - 0x00001FFF (8 KiB);
* Application 0x00002000 - 0x0003FFFF (248 KiB);
* Firmware update 0x00040000 - 0x0007DFFF (248 KiB);
* New firmware record address: 0x0007FF00.

```
New firmware record:

  +-----------------+-----------------+-----------------+---------------+------------+
  |  Major version  |  Minor version  |  Build version  | Firmware type | Magic word |
  | number (1 byte) | number (1 byte) | number (1 byte) |   (1 byte)    | (4 bytes)  |
  +-----------------+-----------------+-----------------+---------------+------------+

```

The bootloader checks if the magic word is equal to 0xAAAAAAAA (firmware update magic word), if it is, the new firmware will be copied from the firmware update region to the application or bootloader region depending on the firmware type (application or bootloader). After finishing the copying, the bootloader will erase the firmware update region.

The Firmware type byte indicates what to update, (0x01: application, 0x02: bootloader). All flash writing logic is executed from SRAM to allow self updating.

## Migrating from the older openMMC versions

newboot is not compatible with openMMC prior version 1.5.0, and the older bootloader doesn't support self update nor openMMC >= 1.5.0, so remote updates via HPM would require a special version of openMMC that updates the bootloader from the application side. This is not done yet, so the only way to safely update the bootloader and openMMC now is via the JTAG/SWD interface.
