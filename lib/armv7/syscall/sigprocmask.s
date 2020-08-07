	.arch armv7a
	.syntax unified

	.section .text

	.global sigprocmask
	.type	sigprocmask, %function

	.align	2

sigprocmask:
	ldr	pc, =_sigprocmask
