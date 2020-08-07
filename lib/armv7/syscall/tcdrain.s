	.arch armv7a
	.syntax unified

	.section .text

	.global tcdrain
	.type	tcdrain, %function

	.align	2

tcdrain:
	ldr	pc, =_tcdrain
