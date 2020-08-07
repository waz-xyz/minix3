	.arch armv7a
	.syntax unified

	.section .text

	.global waitpid
	.type	waitpid, %function

	.align	2

waitpid:
	ldr	pc, =_waitpid
