MEMORY
{
  /* Define each memory region */
  /* First 8kB are reserved for bootloader */
  MFlash128 (rx) : ORIGIN = 0x0000, LENGTH = 8K /* 8K bytes */
  RamLoc16 (rwx) : ORIGIN = 0x10000000, LENGTH = 32K /* 32K bytes */
  RamAHB16 (rwx) : ORIGIN = 0x2007c000, LENGTH = 32K /* 32K bytes */
}
  /* Define a symbol for the top of each memory region */
  __top_MFlash128 = 0x0000 + 8K;
  __top_RamLoc16 = 0x10000000 + 32K;
  __top_RamAHB16 = 0x2007c000 + 32K;

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

    } >MFlash128

    .text : ALIGN(4)
    {
         *(.text*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);

    } > MFlash128

    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this.
     */
    .ARM.extab : ALIGN(4)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > MFlash128
    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > MFlash128
    __exidx_end = .;

    _etext = .;

    /* DATA section for RamAHB16 */
    .data_RAM2 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM2 = .) ;
        *(.ramfunc.$RAM2)
        *(.ramfunc.$RamAHB16)
        *(.data.$RAM2*)
        *(.data.$RamAHB16*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM2 = .) ;
    } > RamAHB16 AT>MFlash128

    /* MAIN DATA SECTION */

    .uninit_RESERVED : ALIGN(4)
    {
        KEEP(*(.bss.$RESERVED*))
        . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > RamLoc16

    /* Main DATA section (RamLoc16) */
    .data : ALIGN(4)
    {
        FILL(0xff)
        _data = . ;
        *(vtable)
        *(.ramfunc*)
        *(.data*)
        . = ALIGN(4) ;
        _edata = . ;
    } > RamLoc16 AT>MFlash128

    /* BSS section for RamAHB16 */
    .bss_RAM2 : ALIGN(4)
    {
        PROVIDE(__start_bss_RAM2 = .) ;
        *(.bss.$RAM2*)
        *(.bss.$RamAHB16*)
        . = ALIGN(4) ;
        PROVIDE(__end_bss_RAM2 = .) ;
    } > RamAHB16

    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > RamLoc16

    /* NOINIT section for RamAHB16 */
    .noinit_RAM2 (NOLOAD) : ALIGN(4)
    {
        *(.noinit_RAM2*)
        *(.noinit_RamAHB16*)
        . = ALIGN(4) ;
    } > RamAHB16

    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        *(.noinit*)
         . = ALIGN(4) ;
        _end_noinit = .;
    } > RamLoc16

    PROVIDE(_pvHeapStart = DEFINED(__user_heap_base) ? __user_heap_base : .);
    PROVIDE(_vStackTop = DEFINED(__user_stack_top) ? __user_stack_top : __top_RamLoc16 - 0);
	PROVIDE(_VectorChecksum = 0 - (_vStackTop + ResetISR + NMI_Handler + HardFault_Handler + MemManage_Handler + BusFault_Handler + UsageFault_Handler + 6 ));
}
