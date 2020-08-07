	.arch armv7a
	.syntax unified

	.section .text

	.global utime
	.type	utime, %function

	.align	2

utime:
	ldr	pc, =_utime
