	.arch armv7a
	.syntax unified

	.section .text

	.global setuid
	.type	setuid, %function

	.global seteuid
	.type	seteuid, %function

	.align	2

setuid:
	ldr	pc, =_setuid

seteuid:
	ldr	pc, =_seteuid
