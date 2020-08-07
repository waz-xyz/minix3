	.arch armv7a
	.syntax unified

	.section .text

	.global setgid
	.type	setgid, %function

	.global setegid
	.type	setegid, %function

	.align	2

setgid:
	ldr	pc, =_setgid

setegid:
	ldr	pc, =_setegid
