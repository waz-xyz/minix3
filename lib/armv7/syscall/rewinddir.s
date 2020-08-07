	.arch armv7a
	.syntax unified

	.section .text

	.global rewinddir
	.type	rewinddir, %function

	.align	2

rewinddir:
	ldr	pc, =_rewinddir
