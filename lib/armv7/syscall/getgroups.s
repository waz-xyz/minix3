	.arch armv7a
	.syntax unified

	.section .text

	.global getgroups
	.type	getgroups, %function

	.align	2

getgroups:
	ldr	pc, =_getgroups
