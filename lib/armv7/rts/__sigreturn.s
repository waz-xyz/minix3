# This routine is the low-level code for returning from signals.  
# It calls _sigreturn, which is the normal "system call" routine.
# Both __sigreturn and _sigreturn are needed.

	.arch armv7a
	.syntax unified

	.section .text

	.global __sigreturn
	.type	__sigreturn, %function

	.align	2
	
__sigreturn:
	@add	esp, 16
	ldr	pc, =_sigreturn
