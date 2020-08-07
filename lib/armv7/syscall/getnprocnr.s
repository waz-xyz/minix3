	.arch armv7a
	.syntax unified

	.section .text

	.global getnprocnr
	.type	getnprocnr, %function

	.align	2

getnprocnr:
	ldr	pc, =_getnprocnr
