	.arch armv7a
	.syntax unified

	.section .text

	.global pipe
	.type	pipe, %function

	.align	2

pipe:
	ldr	pc, =_pipe
