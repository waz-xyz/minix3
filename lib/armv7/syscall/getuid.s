	.arch armv7a
	.syntax unified

	.section .text

	.global getuid
	.type	getuid, %function

	.align	2

getuid:
	ldr	pc, =_getuid
