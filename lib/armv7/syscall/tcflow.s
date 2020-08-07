	.arch armv7a
	.syntax unified

	.section .text

	.global tcflow
	.type	tcflow, %function

	.align	2

tcflow:
	ldr	pc, =_tcflow
