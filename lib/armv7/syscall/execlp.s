	.arch armv7a
	.syntax unified

	.section .text

	.global execlp
	.type	execlp, %function

	.align	2

execlp:
	ldr	pc, =_execlp
