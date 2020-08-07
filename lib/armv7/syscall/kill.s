	.arch armv7a
	.syntax unified

	.section .text

	.global kill
	.type	kill, %function

	.align	2

kill:
	ldr	pc, =_kill
