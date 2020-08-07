	.arch armv7a
	.syntax unified

	.section .text

	.global mkfifo
	.type	mkfifo, %function

	.align	2

mkfifo:
	ldr	pc, =_mkfifo
