	.arch armv7a
	.syntax unified

	.section .text

	.global closedir
	.type	closedir, %function

	.align	2

closedir:
	ldr	pc, =_closedir
