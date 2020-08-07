	.arch armv7a
	.syntax unified

	.section .text

	.global getgid
	.type	getgid, %function

	.align	2

getgid:
	ldr	pc, =_getgid
