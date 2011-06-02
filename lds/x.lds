ENTRY(begin)
SECTIONS
{
	. = 0;
	.text.loader : {
		*(.text.loader)
	}
	. = 0x150;
	.data.loader : {
		*(.data.loader)
	}
	. = 0x1fe;
	.rodata.magic : {
		SHORT(0xaa55)
	}
	. = 0x400;
	.text : {
		*(.text)
	}
	.rodata : {
		*(.rodata)
	}
	.data : {
		*(.data)
	}
	.bss : {
		*(.bss)
	}
}
