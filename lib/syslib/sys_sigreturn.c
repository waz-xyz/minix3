#include "syslib.h"

/*===========================================================================*
 *                                sys_sigreturn				     *
 *===========================================================================*/
PUBLIC int sys_sigreturn(
	int proc_nr,			/* for which process */
	struct sigmsg *sig_ctxt		/* POSIX style handling */
)
{
	message m;
	int result;

	m.SIG_ENDPT = proc_nr;
	m.SIG_CTXT_PTR = (char *)sig_ctxt;
	result = _taskcall(SYSTASK, SYS_SIGRETURN, &m);
	return result;
}
