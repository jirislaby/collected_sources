.code16
.section ".text.head", "ax"

entry_16:
	ljmp $0x07c0, $0f
0:
	jmp 0b

.section ".data.header", "a"
setup_sects:	.byte 1
root_flags:	.word 0
syssize:	.long 0
ram_size:	.word 0
vid_mode:	.word 0
root_dev:	.word 0
boot_flag:	.word 0xaa55
_start:		.byte	0xeb, start_of_setup-1f
1:
		.ascii	"HdrS"
		.word	0x0200
realmode_swtch:	.word	0, 0
start_sys_seg:	.word	0
		.word	kernel_version-512
type_of_loader:	.byte	0
loadflags:	.byte	1
setup_move_size: .word	0x8000
code32_start:	.long	0x100000
ramdisk_image:	.long	0
ramdisk_size:	.long	0
bootsect_kludge: .long	0

.section ".text.entry", "ax"
start_of_setup:
	hlt
	jmp start_of_setup

.section ".rodata.entry"
kernel_version:	.asciz "bubak"

.section ".text", "ax"
.code32

.globl entry_32
entry_32:
	hlt
	jmp entry_32
