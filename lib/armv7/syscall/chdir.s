	.arch armv7a
	.syntax unified

	.section .text

	.global chdir
	.type	chdir, %function

	.global fchdir
	.type	fchdir, %function

	.align	2

chdir:
	ldr	pc, =_chdir

fchdir:
	ldr	pc, =_fchdir
