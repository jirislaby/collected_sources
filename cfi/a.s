.globl asm_fun
asm_fun:
	.cfi_startproc
	pushq %rax
	.cfi_adjust_cfa_offset 8
	call c_fun
	popq %rax
	.cfi_adjust_cfa_offset -8
	ret
	.cfi_endproc
.type asm_fun, @function
.size asm_fun, .-asm_fun
