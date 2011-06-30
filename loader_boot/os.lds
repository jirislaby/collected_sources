OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(entry_32)
SECTIONS
{
	. = 0;
	.text.head : {
		head.o(.text.head)
	}
	.data.head : {
		head.o(.data.head)
	}
	. = 0x1f1;
	.data.header : {
		head.o(.data.header)
	}
	.text.entry : {
		head.o(.text.entry)
	}
	.rodata.entry : {
		head.o(.rodata.entry)
	}
	. = 8192;
	__os_start = .;
	.rodata.os_start_magic : {
		__os_start_magic = .;
		LONG(0xdedaeda0);
	}
	.text : {
		_text = .;
		*(.text)
	}
	.data : {
		*(.data)
	}
	.rodata : {
		*(.rodata)
	}
	.rodata.handlers : {
		__handlers = .;
		*(.rodata.handlers)
		LONG(0);
	}
	.bss : {
		__bss_start = .;
		*(.bss)
		__bss_end = .;
	}
	.rodata.os_end_magic : {
		. = ALIGN(512) + 508;
		__os_end_magic = .;
		LONG(0xdedaedaf);
	}
	__os_end = .;
}
