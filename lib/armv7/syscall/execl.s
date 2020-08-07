	.arch armv7a
	.syntax unified

	.section .text

	.global execl
	.type	execl, %function

	.align	2

execl:
	ldr	pc, =_execl
