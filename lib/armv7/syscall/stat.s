	.arch armv7a
	.syntax unified

	.section .text

	.global stat
	.type	stat, %function

	.align	2

stat:
	ldr	pc, =_stat
