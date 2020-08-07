	.arch armv7a
	.syntax unified

	.section .text

	.global lstat
	.type	lstat, %function

	.align	2

lstat:
	ldr	pc, =_lstat
