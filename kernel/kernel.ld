ENTRY(_start)

physbase = 0x100000;
pagealign = 4096;

SECTIONS
{
	. = physbase;
	. = ALIGN(pagealign);

	__text_start = .;
	.text :
	{
		KEEP(*(.text.boot))
		KEEP(*(.text .text.*))
		. = ALIGN(pagealign);
	}
	__text_end = .;
	etext = .;

	__rodata_start = .;
	.rodata :
	{
		KEEP(*(.rodata .rodata.*))
		. = ALIGN(pagealign);
	}
	__rodata_end = .;
	erodata = .;

	__data_start = .;
	.data :
	{
		KEEP(*(.data .data.*))
		. = ALIGN(pagealign);
	}
	__data_end = .;
	edata = .;
 
	__bss_start = .;
	.bss :
	{
		bss = .;
		KEEP(*(.bss .bss.*))
		KEEP(*(COMMON))
		. = ALIGN(pagealign);
	}
	__bss_end = .;
	ebss = .;

	__end = .;
	end = .;

	/DISCARD/ :
	{
		*(.note.gnu.build-id)
		*(.ARM.attributes)
		*(.comment)
	}
}