/* Linker script to configure memory regions. */

/* !! the include symbole may failed if the symbol file name is too long!! */
INCLUDE "romsym_s.so"

MEMORY
{
  /* !!! These memory map configuration MUST match with the SAU setting !!! */
  /* The vector table, it must start with 256 bytes aligned address */
  VECTORS_RAM (rwx)     : ORIGIN = 0x10000000, LENGTH = 0x100000A0 - 0x10000000

  /* 0x100000A0 ~ 0x10000480 reserved for ROM code */

  /* RAM functions entry table */
  RAM_FUN_TABLE (rwx)   : ORIGIN = 0x10000480, LENGTH = 0x100004F0 - 0x10000480

  /* RAM image Signature */
  RAM_IMG_SIGN (rwx)    : ORIGIN = 0x100004F0, LENGTH = 0x10000500 - 0x100004F0

  /* Internal RAM for program data & text */
  DTCM_RAM (wrx) 		: ORIGIN = 0x10000500, LENGTH = 0x10008000 - 0x10000500

  /* External PSRAM for text, rodata & data */
  PSRAM (rwx)           : ORIGIN = 0x60000000, LENGTH = 0x60100000 - 0x60000000

  /* Flash memory for XIP */
  /* XIP image must start with 64K(0x10000) aligned address */
  /* XIP Chiper section: TEXT/RODATA in this section can be encrypted (decrypt by SCE) */
  XIP_FLASH_C (rx) 		: ORIGIN = 0x9BA80000, LENGTH = 0x9BE00000 - 0x9BA80000
  /* XIP Plantext section: RODATA in this section will not be encrypted */
  XIP_FLASH_P (rx) 		: ORIGIN = 0x9BE00000, LENGTH = 0x9BFC0000 - 0x9BE00000

  /* XIP for NSC (Non-Secure Callable) */
  XIP_FLASH_NSC (rx)     : ORIGIN = 0x9BFC0000, LENGTH = 0x9C000000 - 0x9BFC0000
}

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __Vectors_End
 *   __Vectors_Size
 */
_start_addr = 0x1000;
ENTRY(_start_addr)
__eram_end__ = ORIGIN(PSRAM) + LENGTH(PSRAM);
__psram_start__ = ORIGIN(PSRAM);
__psram_end__ = ORIGIN(PSRAM) + LENGTH(PSRAM);

SECTIONS
{
	.ram.vector :
	{
		. = ALIGN(0x100);
		__ram_vector_start__ = .;
		KEEP(*(.ram_vector_table))
		__ram_vector_end__ = .;
		. = ALIGN(4);
	} > VECTORS_RAM

	.ram.func.table :
	{
		. = ALIGN(0x10);
		__ram_start_table_start__ = .;
		KEEP(*(SORT(.start.ram.data*)))
		__ram_start_table_end__ = .;

	} > RAM_FUN_TABLE

	.ram.img.signature :
	{
		. = ALIGN(0x04);
		__ram_img_signature__ = .;
		KEEP(*(.start.ram.sign*))		
	} > RAM_IMG_SIGN

	/* Non-secure callable function */
	.xip_nsc_reserved :
	{
		/* XIP image section must start with 64K aligned address, reserve 0x140 for image header */
		. += 0x140;
		/* If the XIP is the 1st sub-image, then we should reserve 0xE0 bytes for OTA Sign. and 6 Public Key */
		/*. += 0xE0; */
	} > XIP_FLASH_NSC

	/* for Old ASDK NSC is put in section .sgstubs */
	/* .sgstubs : for Old ASDK */
	.sgstubs :
	{
		/* For xip encrypted section ram image signature */
		KEEP(*(.xip.ram.sign.nsc))
		. = ALIGN(0x20);
		__nsc_start__ = .;
		KEEP(*(.ns_entry_func.text*))
	} > XIP_FLASH_NSC

	.gnu.sgstubs :
	{
		. = ALIGN(0x20);
		KEEP(*(.ns_entry_func.text*))
		__nsc_end__ = .;
	} > XIP_FLASH_NSC

	.psram.data : AT (__psram_etext)
	{
		. = ALIGN(16);
	    __psram_etext = .; 
		__psram_data_start__ = .;

		*(.psram.data*)

		__psram_data_end__ = .;

	} > PSRAM

	.data :
	{
		. = ALIGN(0x20);
		__fw_img_start__ = .;
		__data_start__ = .;
		*(vtable)
		*(.sram.data*)		
		*(.data*)

		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);

		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP(*(.jcr*))
		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;

	} > DTCM_RAM

	.xip_reserved_c :
	{
		/* XIP image section must start with 64K aligned address, reserve 0x140 for image header */
		. += 0x140;
		/* If the XIP is the 1st sub-image, then we should reserve 0xE0 bytes for OTA Sign. and 6 Public Key */
		/*. += 0xE0; */
	} > XIP_FLASH_C

	.xip_reserved_p :
	{
		/* XIP image section must start with 64K aligned address, reserve 0x140 for image header */
		. += 0x140;
		/* If the XIP is the 1st sub-image, then we should reserve 0xE0 bytes for OTA Sign. and 6 Public Key */
		/*. += 0xE0; */
	} > XIP_FLASH_P

	.psram.code_text :
	{
		. = ALIGN(16);
		__psram_code_text_start__ = .;

		*(.psram.text*)
		
		__psram_code_text_end__ = .;
	} > PSRAM

	.psram.code_rodata :
	{
		. = ALIGN(16);
		__psram_code_rodata_start__ = .;

		*(.psram.rodata*)

		. = ALIGN(4);
		__psram_code_rodata_end__ = .;
	} > PSRAM

	.ram.code_text :
	{
		. = ALIGN(32);
		__ram_entry_text_start__ = .;
		*(.ram_entry_func.text*)
		__ram_entry_text_end__ = .;

		. = ALIGN(32);
		__ram_code_text_start__ = .;

		/* SPI flash controller related code should be located in SRAM, never locates them in XIP */
		*hal_spic*.o(.text*)

		/* LPDDR controller related code should be located in SRAM, never locates them in LPDDR */
		*hal_lpcram*.o(.text*)

		*ram_start*.o(.text*)
		*hal_sys_ctrl*.o(.text*)
		*rtl8710c_pinmux_patch*.o(.text*)
		*hal_syson*.o(.text*)
		*hal_pinmux*.o(.text*)
		*hal_efuse*.o(.text*)

		*(.lpddr.text*)
		*(.sram.text*)
		. = ALIGN(4);
		__ram_code_text_end__ = .;
	} > DTCM_RAM

	.ram.code_rodata :
	{
		. = ALIGN(4);
		__ram_code_rodata_start__ = .;

		/* SPI flash controller related code should be located in SRAM, never locates them in XIP */		
		*hal_spic*.o(.rodata*)

		/* LPDDR controller related code should be located in SRAM, never locates them in LPDDR */
		*hal_lpcram*.o(.rodata*)

		*ram_start*.o(.rodata*)	
		*hal_sys_ctrl*.o(.rodata*)
		*rtl8710c_pinmux_patch*.o(.rodata*)
		*hal_syson*.o(.rodata*)
		*hal_pinmux*.o(.rodata*)
		*hal_efuse*.o(.rodata*)

		. = ALIGN(4);
		__ram_code_rodata_end__ = .;
	} > DTCM_RAM

	.xip.code_c :
	{
		/* For xip encrypted section ram image signature */
		KEEP(*(.xip.ram.sign.s))
		/* code and RO data in this section will be encrypted */
		. = ALIGN(16);
		__xip_code_text_start__ = .;

		*(.xip.text*)

		*(.text*)

		/* put RO data sections need to be encrypted here */
		*(.xip.sec_rodata*)
		
		__xip_code_text_end__ = .;
	} > XIP_FLASH_C

	.xip.code_p :
	{
		/* code and RO data in this section will NOT be encrypted */
		/* put DMA RO data here */
		__xip_code_rodata_start__ = .;

		*(.xip.rodata*)

		*(.rodata*)
		*(.rodata.str1*)
		
		. = ALIGN(4);
		__xip_code_rodata_end__ = .;
	} > XIP_FLASH_P

	.system_restore_data :
	{
		/* data in this section will not be initialed by ram_start() */
		/* some of them will be initialed by boot loader */
		. = ALIGN(32);
		*(.sys_restore.bss*)
		*(.ram.bss.noinit*)
	} > DTCM_RAM
	
	.psram.bss :
	{
		. = ALIGN(16);
		__psram_bss_start__ = .;

		*(.psram.bss*)

		__psram_bss_end__ = .;

		. = ALIGN(32);
		__eram_bss_end__ = .;
	} > PSRAM
	
	.bss :
	{
		. = ALIGN(16);
		__bss_start__ = .;
		*(.sram.bss*)
		*(.bss*)
		*(COMMON)

		. = ALIGN(32);
		__bss_end__ = .;
	} > DTCM_RAM

	.heap (COPY):
	{
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		KEEP(*(.heap*))
		__HeapLimit = .;
	} > DTCM_RAM

	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (COPY):
	{
		KEEP(*(.stack*))
	} > DTCM_RAM

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(DTCM_RAM) + LENGTH(DTCM_RAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);
	PROVIDE(__sram_end__ = __StackLimit);

	/* Check if data + heap + stack exceeds RAM limit */
/* TODO:	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack") */
}
