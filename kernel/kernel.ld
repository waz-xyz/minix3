ENTRY(_start)

virtbase = 0x80000000;
smallpagealign = 0x1000;
largepagealign = 0x10000;

PHDRS
{
	seg_text PT_LOAD;
	seg_data PT_LOAD;
}

SECTIONS
{
	. = virtbase;

	__text_start = .;

	.text ALIGN(largepagealign) :
	{
		KEEP(*(.text.boot))
		KEEP(*(.text .text.*))
	} : seg_text
	
	__text_end = .;
	etext = .;

	__rodata_start = .;

	.rodata ALIGN(32) :
	{
		KEEP(*(.rodata .rodata.*))
	} : seg_text
	
	__rodata_end = .;
	erodata = .;

	.data ALIGN(largepagealign) :
	{
		__data_start = .;
		KEEP(*(.data .data.*))
	} : seg_data

	__data_end = .;
	edata = .;

	__bss_start = .;
	bss = .;

	.bss ALIGN(32) :
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