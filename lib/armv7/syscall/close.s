	.arch armv7a
	.syntax unified

	.section .text

	.global close
	.type	close, %function

	.align	2

close:
	ldr	pc, =_close
