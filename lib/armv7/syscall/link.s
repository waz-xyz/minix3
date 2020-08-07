	.arch armv7a
	.syntax unified

	.section .text

	.global link
	.type	link, %function

	.align	2

link:
	ldr	pc, =_link
