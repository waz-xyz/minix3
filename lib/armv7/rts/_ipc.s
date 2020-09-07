	.arch armv7a
	.syntax unified

	.section .text

	.global _echo
	.type	_echo, %function

	.global _notify
	.type	_notify, %function

	.global _send
	.type	_send, %function

	.global _receive
	.type	_receive, %function

	.global _sendrec
	.type	_sendrec, %function

	.align	2

# See src/kernel/ipc.h for C definitions
SEND = 1
RECEIVE = 2
SENDREC = 3 
NOTIFY = 4
ECHO = 8
SYSVEC = 33			// trap to kernel 

/*========================================================================*
 *                           IPC assembly routines			  *
 *========================================================================*
 * all message passing routines follow the regular calling conventions;
 * SVC calls pass messages through registers r4 to r12 but don't touch
 * any other.
 */

_send:
	// r0 = destination
	// r1 = message pointer
	mov	r3, SEND		// _send(dest, ptr)
	svc	SYSVEC			// trap to the kernel
	bx	lr

_receive:
	push	{r4-r12, lr}
	// r0 = src
	// r1 = message pointer
	mov	r3, RECEIVE		// _receive(src, ptr)
	svc	SYSVEC			// trap to the kernel
	stm	r1, {r4-r12}
	pop	{r4-r12, pc}

_sendrec:
	push	{r4-r12, lr}
	// r0 = dest-src
	// r1 = message pointer
	mov	r3, SENDREC		// _sendrec(srcdest, ptr)
	svc	SYSVEC			// trap to the kernel
	stm	r1, {r4-r12}
	pop	{r4-r12, pc}

_notify:
	// r0 = destination 
	mov	r3, NOTIFY		// _notify(srcdst)
	svc	SYSVEC			// trap to the kernel
	bx	lr

_echo:
	push	{r4-r12, lr}
	mov	r1, r0			// r1 = message pointer
	mov	r3, ECHO		// _echo(ptr)
	svc	SYSVEC			// trap to the kernel
	stm	r1, {r4-r12}
	pop	{r4-r12, pc}
