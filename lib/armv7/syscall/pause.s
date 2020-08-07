	.arch armv7a
	.syntax unified

	.section .text

	.global pause
	.type	pause, %function

	.align	2

pause:
	ldr	pc, =_pause
