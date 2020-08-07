	.arch armv7a
	.syntax unified

	.section .text

	.global getprocnr
	.type	getprocnr, %function

	.align	2

getprocnr:
	ldr	pc, =_getprocnr
