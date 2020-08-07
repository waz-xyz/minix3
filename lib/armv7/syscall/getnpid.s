	.arch armv7a
	.syntax unified

	.section .text

	.global getnpid
	.type	getnpid, %function

	.align	2

getnpid:
	ldr	pc, =_getnpid
