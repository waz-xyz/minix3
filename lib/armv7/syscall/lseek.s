	.arch armv7a
	.syntax unified

	.section .text

	.global lseek
	.type	lseek, %function

	.align	2

lseek:
	ldr	pc, =_lseek
