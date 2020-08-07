	.arch armv7a
	.syntax unified

	.section .text

	.global setsid
	.type	setsid, %function

	.align	2

setsid:
	ldr	pc, =_setsid
