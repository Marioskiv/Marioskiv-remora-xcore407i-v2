/*
******************************************************************************
**
**  File        : STM32F407IGTx_FLASH.ld
**
**  Abstract    : Linker script for STM32F407IG Device (1024KB Flash, 192KB RAM)
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32F407IGT6 (XCore407I board)
**
**  Distribution: The file is distributed "as is," without any warranty
**                of any kind.
**
******************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x400;      /* required amount of heap  */
_Min_Stack_Size = 0x800;     /* required amount of stack */

/* Flash sizes - Remora needs JSON config regions */
_FLASH_TOTAL_SIZE = 1024K;
_FLASH_BOOT_SIZE = 16K;  
_FLASH_JSON_UPLOAD_SIZE = 16K;
_FLASH_JSON_STORAGE_SIZE = 16K;
_PGM_FLASH_AVAILABLE_SIZE = _FLASH_TOTAL_SIZE - _FLASH_BOOT_SIZE - _FLASH_JSON_UPLOAD_SIZE - _FLASH_JSON_STORAGE_SIZE;

/* Specify the memory areas */
MEMORY
{
  RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 128K
  CCMRAM (xrw)   : ORIGIN = 0x10000000, LENGTH = 64K
  FLASH_BOOT (rx)     : ORIGIN = 0x08000000, LENGTH = _FLASH_BOOT_SIZE         /* Sector 0 */
  JSON_UPLOAD  (rx)   : ORIGIN = 0x08004000, LENGTH = _FLASH_JSON_UPLOAD_SIZE  /* Sector 1 */
  JSON_STORAGE (rx)   : ORIGIN = 0x08008000, LENGTH = _FLASH_JSON_STORAGE_SIZE /* Sector 2 */  
  FLASH (rx)          : ORIGIN = 0x0800C000, LENGTH = _PGM_FLASH_AVAILABLE_SIZE
}

/* JSON config locations for Remora */
_ls_json_upload_start = ORIGIN(JSON_UPLOAD);
_ls_json_upload_end   = ORIGIN(JSON_UPLOAD) + LENGTH(JSON_UPLOAD);
_ls_json_storage_start = ORIGIN(JSON_STORAGE);
_ls_json_storage_end   = ORIGIN(JSON_STORAGE) + LENGTH(JSON_STORAGE);
_ls_json_upload_sector  = 1;  /* FLASH_SECTOR_1 */
_ls_json_storage_sector = 2;  /* FLASH_SECTOR_2 */

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH_BOOT */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH_BOOT

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH
  
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT> FLASH

  _siccmram = LOADADDR(.ccmram);

  /* CCM-RAM section */
  .ccmram :
  {
    . = ALIGN(4);
    _sccmram = .;       /* create a global symbol at ccmram start */
    *(.ccmram)
    *(.ccmram*)
    
    . = ALIGN(4);
    _eccmram = .;       /* create a global symbol at ccmram end */
  } >CCMRAM AT> FLASH

  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
