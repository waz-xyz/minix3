	.arch armv7a
	.syntax unified

	.section .text

	.global getpid
	.type	getpid, %function

	.align	2

getpid:
	ldr	pc, =_getpid
