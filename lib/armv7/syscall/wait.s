	.arch armv7a
	.syntax unified

	.section .text

	.global wait
	.type	wait, %function

	.align	2

wait:
	ldr	pc, =_wait
