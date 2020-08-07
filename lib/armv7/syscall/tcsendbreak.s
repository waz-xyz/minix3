	.arch armv7a
	.syntax unified

	.section .text

	.global tcsendbreak
	.type	tcsendbreak, %function

	.align	2

tcsendbreak:
	ldr	pc, =_tcsendbreak
