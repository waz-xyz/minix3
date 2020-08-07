	.arch armv7a
	.syntax unified

	.section .text

	.global getpprocnr
	.type	getpprocnr, %function

	.align	2

getpprocnr:
	ldr	pc, =_getpprocnr
