	.arch armv7a
	.syntax unified

	.section .text

	.global rmdir
	.type	rmdir, %function

	.align	2

rmdir:
	ldr	pc, =_rmdir
