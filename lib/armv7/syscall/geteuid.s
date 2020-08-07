	.arch armv7a
	.syntax unified

	.section .text

	.global geteuid
	.type	geteuid, %function

	.align	2

geteuid:
	ldr	pc, =_geteuid
