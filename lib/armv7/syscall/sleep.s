	.arch armv7a
	.syntax unified

	.section .text

	.global sleep
	.type	sleep, %function

	.align	2

sleep:
	ldr	pc, =_sleep
