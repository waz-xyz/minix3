	.arch armv7a
	.syntax unified

	.section .text

	.global chown
	.type	chown, %function

	.align	2

chown:
	ldr	pc, =_chown
