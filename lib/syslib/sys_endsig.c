#include "syslib.h"

/*===========================================================================*
 *                                sys_endksig				     *
 *===========================================================================*/
PUBLIC int sys_endksig(
	int proc_nr			/* process number */
)
{
	message m;
	int result;

	m.SIG_ENDPT = proc_nr;
	result = _taskcall(SYSTASK, SYS_ENDKSIG, &m);
	return result;
}
