	.arch armv7a
	.syntax unified

	.section .text

	.global _ipc_request
	.type	_ipc_request, %function

	.global _ipc_reply
	.type	_ipc_reply, %function

	.global _ipc_notify
	.type	_ipc_notify, %function

	.global _ipc_receive
	.type	_ipc_receive, %function

	.align	2

// See src/kernel/ipc.h for C definitions.
IPC_REQUEST = 16		// each gets a distinct bit
IPC_REPLY = 32
IPC_NOTIFY = 64
IPC_RECEIVE = 128

SYSVEC = 33			// trap to kernel 

// Offsets of arguments relative to stack pointer.
SRC_DST = 8			// source/ destination process 
SEND_MSG = 12			// message pointer for sending 
EVENT_SET = 12			// notification event set 
RECV_MSG = 16			// message pointer for receiving 


/*========================================================================*
 *                           IPC assembly routines			  *
 *========================================================================*
 * all message passing routines follow the regular calling conventions;
 * SVC calls pass messages through registers r4 to r12 but don't touch
 * any other.
 */

_ipc_request:
	push	{r4-r12}
	// r0 = destination
	// r1 = message pointer
	mov	r3, IPC_REQUEST		// _ipc_request(dst, ptr)
	ldm	r1, {r4-r12}
	svc	SYSVEC			// trap to the kernel
	stm	r1, {r4-r12}
	pop	{r4-r12}
	bx	lr

_ipc_reply:
	push	{r4-r12}
	// r0 = destination
	// r1 = message pointer
	mov	r3, IPC_REPLY		// _ipc_reply(dst, ptr)
	ldm	r1, {r4-r12}
	svc	SYSVEC			// trap to the kernel
	pop	{r4-r12}
	bx	lr

_ipc_receive:
	push	{r4-r12}
	// r0 = src
	// r1 = message pointer
	// r2 = event set
	mov	r3, IPC_RECEIVE		// _ipc_receive(src, ptr, events)
	svc	SYSVEC			// trap to the kernel
	stm	r1, {r4-r12}
	pop	{r4-r12}
	bx	lr

_ipc_notify:
	// r0 = destination
	mov	r2, r1			// r2 = event set 
	mov	r3, IPC_NOTIFY		// _ipc_notify(dst, events)
	svc	SYSVEC			// trap to the kernel
	bx	lr
