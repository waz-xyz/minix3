	.arch armv7a
	.syntax unified

	.section .text

	.global getpgrp
	.type	getpgrp, %function

	.align	2

getpgrp:
	ldr	pc, =_getpgrp
