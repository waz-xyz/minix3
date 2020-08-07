	.arch armv7a
	.syntax unified

	.section .text

	.global dup2
	.type	dup2, %function

	.align	2

dup2:
	ldr	pc, =_dup2
