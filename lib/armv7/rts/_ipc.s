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
SYSVEC = 33			@ trap to kernel 

SRC_DST = 8			@ source/ destination process 
ECHO_MESS = 8			@ echo doesn't have SRC_DST 
MESSAGE = 12			@ message pointer 

#*========================================================================*
#                           IPC assembly routines			  *
#*========================================================================*
# all message passing routines save ebp, but destroy eax and ecx.

_send:
	@ push	ebp
	@ mov	ebp, esp
	@ push	ebx
	@ mov	eax, SRC_DST(ebp)	# eax = dest-src
	@ mov	ebx, MESSAGE(ebp)	# ebx = message pointer
	@ mov	ecx, SEND		# _send(dest, ptr)
	@ int	SYSVEC			# trap to the kernel
	@ pop	ebx
	@ pop	ebp
	bx	lr

_receive:
	@ push	ebp
	@ mov	ebp, esp
	@ push	ebx
	@ mov	eax, SRC_DST(ebp)	# eax = dest-src
	@ mov	ebx, MESSAGE(ebp)	# ebx = message pointer
	@ mov	ecx, RECEIVE		# _receive(src, ptr)
	@ int	SYSVEC			# trap to the kernel
	@ pop	ebx
	@ pop	ebp
	bx	lr

_sendrec:
	@ push	ebp
	@ mov	ebp, esp
	@ push	ebx
	@ mov	eax, SRC_DST(ebp)	# eax = dest-src
	@ mov	ebx, MESSAGE(ebp)	# ebx = message pointer
	@ mov	ecx, SENDREC		# _sendrec(srcdest, ptr)
	@ int	SYSVEC			# trap to the kernel
	@ pop	ebx
	@ pop	ebp
	bx	lr

_notify:
	@ push	ebp
	@ mov	ebp, esp
	@ push	ebx
	@ mov	eax, SRC_DST(ebp)	# ebx = destination 
	@ mov	ecx, NOTIFY		# _notify(srcdst)
	@ int	SYSVEC			# trap to the kernel
	@ pop	ebx
	@ pop	ebp
	bx	lr

_echo:
	@ push	ebp
	@ mov	ebp, esp
	@ push	ebx
	@ mov	ebx, ECHO_MESS(ebp)	# ebx = message pointer
	@ mov	ecx, ECHO		# _echo(srcdest, ptr)
	@ int	SYSVEC			# trap to the kernel
	@ pop	ebx
	@ pop	ebp
	bx	lr
