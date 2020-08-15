ENTRY(_start)

virtbase = 0x80000000;
pagealign = 0x10000;

PHDRS
{
	seg_text PT_LOAD;
	seg_data PT_LOAD;
}

SECTIONS
{
	. = virtbase;
	. = ALIGN(pagealign);

	__text_start = .;
	
	.text :
	{
		KEEP(*(.text.boot))
		KEEP(*(.text .text.*))
	} : seg_text
	
	__text_end = .;
	etext = .;

	. = ALIGN(32);

	__rodata_start = .;
	
	.rodata :
	{
		KEEP(*(.rodata .rodata.*))
	} : seg_text
	
	__rodata_end = .;
	erodata = .;

	. = ALIGN(pagealign);

	__data_start = .;

	.data :
	{
		KEEP(*(.data .data.*))
	} : seg_data

	__data_end = .;
	edata = .;
 
	. = ALIGN(32);

	__bss_start = .;
	bss = .;
	.bss :
	{
		KEEP(*(.bss .bss.*))
		KEEP(*(COMMON))
	} : seg_data

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