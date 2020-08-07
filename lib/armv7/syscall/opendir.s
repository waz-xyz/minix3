	.arch armv7a
	.syntax unified

	.section .text

	.global opendir
	.type	opendir, %function

	.align	2

opendir:
	ldr	pc, =_opendir
