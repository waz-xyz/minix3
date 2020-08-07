	.arch armv7a
	.syntax unified

	.section .text

	.global creat
	.type	creat, %function

	.align	2

creat:
	ldr	pc, =_creat
