/* The <setjmp.h> header relates to the C phenomenon known as setjmp/longjmp.
 * It is used to escape out of the current situation into a previous one.
 * A typical example is in an editor, where hitting DEL breaks off the current
 * command and puts the editor back in the main loop, though care has to be
 * taken when the DEL occurs while executing a library function, since
 * some of them are not reentrant.
 *
 * POSIX does not require the process signal mask to be saved and restored
 * during setjmp/longjmp.  However, the current implementation does this
 * in order to agree with OSF/1 and other BSD derived systems.
 *
 * The pair of functions _setjmp/_longjmp may be used when the signal
 * mask is not to be saved/restored.  These functions are traditional
 * in BSD systems.
 *
 * There are different ways of implementing setjmp/longjmp.  Probably
 * the best way is to unify it with signal handling.  This is true for the
 * following reasons:  Both setjmp/longjmp and signal delivery must save 
 * a context so that it may be restored later.  The jmp_buf necessarily 
 * contains signal information, namely the signal mask to restore.  Both
 * longjmp and the return of a signal handler must trap to the operating
 * system to restore the previous signal mask.  Finally, the jmp_buf
 * and the sigcontext structure contain the registers to restore.
 *
 * Some compilers, namely ACK, will not enregister any variables inside a
 * function containing a call to setjmp, even if those variables are
 * explicitly declared as register variables.  Thus for ACK, the
 * identification of the jmp_buf with a sigcontext structure would cause
 * unnecessary overhead: the jmp_buf has room for all the registers, but
 * the only registers that need to be saved are the stack pointer, 
 * frame pointer, and program counter.
 *
 * So, for ACK a jmp_buf is much smaller than a sigcontext structure, and
 * longjmp does not directly call sigreturn.  Instead, longjmp calls a
 * front-end function which initializes the appropriate fields of a
 * sigcontext structure, marks this structure as containing no valid
 * general purpose registers, and then calls sigreturn.
 *
 * The POSIX sigjmp_buf is identical to the jmp_buf in all cases.
 *
 * Different compilers have different symbols that they recognize as
 * setjmp symbols.  ACK recognizes __setjmp, the GNU C compiler
 * recognizes setjmp and _setjmp, and BCC recognizes all three.
 * When these symbols occur within a function, the compiler may keep 
 * all local variables on the stack, avoid certain optimizations, or
 * pass hidden arguments to the setjmp function.
 *  
 * Thus, setjmp implementations vary in two independent ways which may
 * be identified through the following preprocessor tokens:
 *
 * _SETJMP_SYMBOL -- If 0, this means the compiler treats setjmp and _setjmp
 * specially.  If 1, this means the compiler treats __setjmp specially.
 *
 * _SETJMP_SAVES_REGS -- If 1, this means setjmp/longjmp must explicitly
 * save and restore all registers.  This also implies that a jmp_buf is
 * different than a sigcontext structure.  If 0, this means that the compiler
 * will not use register variables within a function that calls one of 
 * its SETJMP_SYMBOLs. 
 * 
 * When _SETJMP_SYMBOL = 1, the implementation has a few dozen bytes of
 * unnecessary overhead.  This happens in the following manner:  a program uses
 * _setjmp/_longjmp because it is not interested in saving and restoring the
 * signal mask. Nevertheless, because _setjmp expands to the general purpose
 * function __setjmp, code for sigprocmask(2) is linked into the program.  
 */

#ifndef _SETJMP_H
#define _SETJMP_H

#ifndef _ANSI_H
#include <ansi.h>
#endif

/* The jmp_buf data type.  Do not change the order of these fields -- some
 * C library code refers to these fields by name.  When _SETJMP_SAVES_REGS
 * is 1, the file <sys/jmp_buf.h> gives the usage of the sixteen registers.
 */
typedef unsigned long long __jmp_buf[32];

typedef struct __jmp_buf_tag {
	__jmp_buf __jb;
	unsigned long __fl;
	unsigned long __ss[128/sizeof(long)];
} jmp_buf[1];

int setjmp (jmp_buf);
_Noreturn void longjmp (jmp_buf, int);

#ifdef _MINIX
int _setjmp (jmp_buf);
_Noreturn void _longjmp (jmp_buf, int);
#endif

#ifdef _POSIX_SOURCE
typedef jmp_buf sigjmp_buf;
int sigsetjmp (sigjmp_buf, int);
_Noreturn void siglongjmp (sigjmp_buf, int);
#endif /* _POSIX_SOURCE */

#define setjmp setjmp

#endif /* _SETJMP_H */
