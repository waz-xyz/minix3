	.arch armv7a
	.syntax unified

	.section .text

	.global getppid
	.type	getppid, %function

	.align	2

getppid:
	ldr	pc, =_getppid
