	.arch armv7a
	.syntax unified

	.section .text

	.global svrctl
	.type	svrctl, %function

	.align	2

svrctl:
	ldr	pc, =_svrctl
