	.arch armv7a
	.syntax unified

	.section .text

	.global fcntl
	.type	fcntl, %function

	.align	2

fcntl:
	ldr	pc, =_fcntl
