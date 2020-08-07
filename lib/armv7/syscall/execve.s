	.arch armv7a
	.syntax unified

	.section .text

	.global execve
	.type	execve, %function

	.align	2

execve:
	ldr	pc, =_execve
