	.arch armv7a
	.syntax unified

	.section .text

	.global uname
	.type	uname, %function

	.align	2

uname:
	ldr	pc, =_uname
