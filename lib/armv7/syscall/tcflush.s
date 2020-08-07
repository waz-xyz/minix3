	.arch armv7a
	.syntax unified

	.section .text

	.global tcflush
	.type	tcflush, %function

	.align	2

tcflush:
	ldr	pc, =_tcflush
