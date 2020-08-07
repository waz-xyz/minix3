	.arch armv7a
	.syntax unified

	.section .text

	.global execvp
	.type	execvp, %function

	.align	2

execvp:
	ldr	pc, =_execvp
