	.arch armv7a
	.syntax unified

	.section .text

	.global ptrace
	.type	ptrace, %function

	.align	2

ptrace:
	ldr	pc, =_ptrace
