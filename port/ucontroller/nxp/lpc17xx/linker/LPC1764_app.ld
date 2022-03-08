MEMORY
{
  /* Define each memory region */
  /* Last 2 sectors (32kB each) are reserved for firmware upgrade */
  BootFlash (r)      : ORIGIN = 0x0000, LENGTH = 8K /* 8K bytes */
  AppFlash (rx)      : ORIGIN = 0x2000, LENGTH = 56K /* 56K bytes */
  FWUpdateFlash (r)  : ORIGIN = 0x10000, LENGTH = 64K /* 64K bytes */
  RamLoc (rwx)       : ORIGIN = 0x10000000, LENGTH = 16K /* 16K bytes */
  RamAHB (rwx)       : ORIGIN = 0x2007c000, LENGTH = 16K /* 16K bytes */
}
  __Bootloader_start    = 0x0000; /* Bootloader start address (vector table) */
  __Bootloader_end      = 0x1FFF; /* Bootloader end address (last byte) */
  __AppFlash_start      = 0x2000; /* Application start address (vector table) */
  __AppFlash_end        = 0x1FFF + 56K;  /* Application end address (last byte) */
  __FWUpdateFlash_start = 0x10000; /* Firmware update region start address */
  __FWUpdateFlash_end   = 0x0FFFF + 64K; /* Firmware update region end address (last byte) */

  /* Last 256 bytes of flash reserved to the fw_info struct */
  __FWInfo_addr         = __FWUpdateFlash_end - 255;

  /* Define a symbol for the top of each memory region */
  __top_AppFlash = 0x2000 + 56K;
  __top_RamLoc = 0x10000000 + 16K;
  __top_RamAHB = 0x2007c000 + 16K;

ENTRY(ResetISR)

SECTIONS
{

    /* MAIN TEXT SECTION */
    .text : ALIGN(4)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))

        /* Global Section Table */
        . = ALIGN(4) ;
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        LONG(LOADADDR(.data_RAM2));
        LONG(    ADDR(.data_RAM2));
        LONG(  SIZEOF(.data_RAM2));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        LONG(    ADDR(.bss_RAM2));
        LONG(  SIZEOF(.bss_RAM2));
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */

        *(.after_vectors*)

    } > AppFlash

    .text : ALIGN(4)
    {
         *(.text*)
        *(.rodata .rodata.* .constdata .constdata.*)
       /* . = ALIGN(4); */

    } > AppFlash

    .ipmi_handlers : ALIGN(32)
    {
        _ipmi_handlers = .;
        KEEP(*(.ipmi_handlers))
        _eipmi_handlers = .;
    } > AppFlash

    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this.
     */
    .ARM.extab : ALIGN(4)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > AppFlash
    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > AppFlash
    __exidx_end = .;

    _etext = .;

    /* DATA section for RamAHB */
    .data_RAM2 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM2 = .) ;
        *(.ramfunc.$RAM2)
        *(.ramfunc.$RamAHB)
        *(.data.$RAM2*)
        *(.data.$RamAHB*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM2 = .) ;
    } > RamAHB AT> AppFlash

    /* MAIN DATA SECTION */

    .uninit_RESERVED : ALIGN(4)
    {
        KEEP(*(.bss.$RESERVED*))
        . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > RamLoc

    /* Main DATA section (RamLoc) */
    .data : ALIGN(4)
    {
        FILL(0xff)
        _data = . ;
        *(vtable)
        *(.ramfunc*)
        *(.data*)
        . = ALIGN(4) ;
        _edata = . ;
    } > RamLoc AT> AppFlash

    /* BSS section for RamAHB */
    .bss_RAM2 : ALIGN(4)
    {
        PROVIDE(__start_bss_RAM2 = .) ;
        *(.bss.$RAM2*)
        *(.bss.$RamAHB*)
        . = ALIGN(4) ;
        PROVIDE(__end_bss_RAM2 = .) ;
    } > RamAHB

    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > RamLoc

    /* NOINIT section for RamAHB */
    .noinit_RAM2 (NOLOAD) : ALIGN(4)
    {
        *(.noinit_RAM2*)
        *(.noinit_RamAHB*)
        . = ALIGN(4) ;
    } > RamAHB

    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        *(.noinit*)
         . = ALIGN(4) ;
        _end_noinit = .;
    } > RamLoc

    PROVIDE(_pvHeapStart = DEFINED(__user_heap_base) ? __user_heap_base : .);
    PROVIDE(_vStackTop = DEFINED(__user_stack_top) ? __user_stack_top : __top_RamLoc - 0);

    /* Add 6 to the sum to compensate for the lacking of the less
    significant bit (thumb mode) */

    PROVIDE(_VectorChecksum = 0 - (_vStackTop + ResetISR + NMI_Handler + HardFault_Handler + MemManage_Handler + BusFault_Handler + UsageFault_Handler + 6 ));
}
