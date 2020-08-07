	.arch armv7a
	.syntax unified

	.section .text

	.global getegid
	.type	getegid, %function

	.align	2

getegid:
	ldr	pc, =_getegid
