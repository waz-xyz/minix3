	.arch armv7a
	.syntax unified

	.section .text

	.global sigaddset
	.type	sigaddset, %function

	.align	2

sigaddset:
	ldr	pc, =_sigaddset
