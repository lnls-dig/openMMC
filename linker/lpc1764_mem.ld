/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2015
 * (c) NXP Semiconductors 2013-2015
 * Linker script memory definitions
 * Created from LinkMemoryTemplate
 * By LPCXpresso v7.8.0 [Build 426] [2015-05-28]  on Thu Jul 30 09:03:05 BRT 2015)
*/

MEMORY
{
  /* Define each memory region */
  MFlash128 (rx) : ORIGIN = 0x0, LENGTH = 0x20000 /* 128K bytes */
  RamLoc16 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x4000 /* 16K bytes */
  RamAHB16 (rwx) : ORIGIN = 0x2007c000, LENGTH = 0x4000 /* 16K bytes */
}
  /* Define a symbol for the top of each memory region */
  __top_MFlash128 = 0x0 + 0x20000;
  __top_RamLoc16 = 0x10000000 + 0x4000;
  __top_RamAHB16 = 0x2007c000 + 0x4000;



