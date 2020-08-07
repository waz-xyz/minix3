	.arch armv7a
	.syntax unified

	.section .text

	.global sigemptyset
	.type	sigemptyset, %function

	.align	2

sigemptyset:
	ldr	pc, =_sigemptyset
