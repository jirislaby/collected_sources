.text

#define BRK_SZ 0x1000
#define MMAP_SZ (100 << 20)

.global _start
_start:
	movb $'b', %al
	call write
	call alloc_brk

	push %rax
	movb $'z', %al
	call write
	pop %rdi
	mov $BRK_SZ, %rcx
	call memzero

	movb $'m', %al
	call write
	call alloc_mmap

	push %rax
	movb $'z', %al
	call write
	pop %rdi
	mov $MMAP_SZ, %rcx
	call memzero

	movb $'s', %al
	call write
	call nanosleep
	jmp _start

loop:
	call nanosleep
	jmp loop

# exit
	mov $5, %rdi
	mov $60, %rax
	syscall

nanosleep:
# nanosleep
	lea sleep_req, %rdi
	xor %rsi, %rsi
	mov $35, %rax
	syscall
	ret

alloc_brk:
# brk
	mov $0, %rdi
	mov $12, %rax
	syscall
	push %rax

# brk
	mov %rax, %rdi
	add $BRK_SZ, %rdi
	mov $12, %rax
	syscall
	pop %rax
	ret

alloc_mmap:
# mmap
	xor %rdi, %rdi
	mov $MMAP_SZ, %rsi
#	mov $1<<40, %rsi
	mov $3, %rdx # R+W
	mov $0x22, %r10 # MAP_PRIVATE|MAP_ANONYMOUS
	mov $-1, %r8
	xor %r9, %r9
#	mov $10, %r9
	mov $9, %rax # MAP_PRIVATE|MAP_ANONYMOUS
	syscall
	cmpq $0xfffffffffffff000, %rax
	ja 1f
	ret
# write & die
1:
	negq %rax
	add $'A', %al
	call write
2:
	jmp 2b

memzero:
	xor %al, %al
	rep; stosb
	ret

write:
	mov %al, letter
	mov $1, %rdi
	lea text, %rsi
	mov $text_len, %rdx
	mov $1, %rax
	syscall # syscall 1, 1, &text, text_len

	movb counter, %al
	incb %al
	cmpb $0x30 + 10, %al
	jb 1f
	movb $0x30, %al
1:
	movb %al, counter
	ret

.data
text:
	.ascii "ahoj "
letter:
	.byte 'x'
	.byte ' '
counter:
       	.byte '0'
	.byte '\n'
text_len = . - text

sleep_req:
	.quad 0
	.quad 500000000
