	.arch armv7a
	.syntax unified

	.section .text

	.global getsysinfo
	.type	getsysinfo, %function

	.align	2

getsysinfo:
	ldr	pc, =_getsysinfo
