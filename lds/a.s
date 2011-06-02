.section .text.loader
.globl begin
begin:
.byte 0xea
.word start, 0x7c0

start:
cli
cld

hlt
jmp start

.section .data.loader
