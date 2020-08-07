	.arch armv7a
	.syntax unified

	.section .text

	.global reboot
	.type	reboot, %function

	.align	2

reboot:
	ldr	pc, =_reboot
