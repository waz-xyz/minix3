	.arch armv7a
	.syntax unified

	.section .text

	.global _pm_findproc
	.type	_pm_findproc, %function

	.align	2

_pm_findproc:
	ldr	pc, =__pm_findproc
