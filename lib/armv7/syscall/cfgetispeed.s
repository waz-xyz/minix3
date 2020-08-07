	.arch armv7a
	.syntax unified

	.section .text

	.global cfgetispeed
	.type	cfgetispeed, %function

	.align	2


cfgetispeed:
	ldr	pc, =_cfgetispeed
