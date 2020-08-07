	.arch armv7a
	.syntax unified

	.section .text

	.global fork
	.type	fork, %function

	.align	2

fork:
	ldr	pc, =_fork
