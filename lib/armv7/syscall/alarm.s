	.arch armv7a
	.syntax unified

	.section .text

	.global alarm
	.type	alarm, %function

	.align	2

alarm:
	ldr	pc, =_alarm
