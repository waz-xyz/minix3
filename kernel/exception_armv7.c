/* This file contains a simple exception handler.  Exceptions in user
 * processes are converted to signals. Exceptions in a kernel task cause
 * a panic.
 */

#include "kernel.h"
#include <signal.h>
#include "proc.h"

struct ex_s
{
	const char *msg;
	int signum;
};
static struct ex_s ex_data[] = {
	[RESET_EXCEPTION] = {"Reset", SIGBUS},
	[UNDEFINED_INSTRUCTION_EXCEPTION] = {"Undefined instruction", SIGILL},
	[SUPERVISOR_CALL_EXCEPTION] = {"Supervisor call", SIGTRAP},
	[PREFETCH_ABORT_EXCEPTION] = {"Prefetch abort", SIGSEGV},
	[DATA_ABORT_EXCEPTION] = {"Data abort", SIGSEGV},
	[IRQ_EXCEPTION] = {"IRQ", 0},
	[FAST_IRQ_EXCEPTION] = {"Fast IRQ", 0}
};

/*===========================================================================*
 *				exception				     *
 *===========================================================================*/
PUBLIC void exception(unsigned exception_type)
{
	register struct ex_s *ep;
	struct proc *saved_proc;

	/* An exception or interrupt has occurred. */

	if (exception_type < RESET_EXCEPTION || exception_type > FAST_IRQ_EXCEPTION)
	{
		panic("unknown exception type", NO_NUM);
	}

	/* Save proc_ptr, because it may be changed by debug statements. */
	saved_proc = proc_ptr;

	ep = &ex_data[exception_type];

	/* If an exception occurs while running a process, the k_reenter variable 
	* will be zero. Exceptions in interrupt handlers or system traps will make 
	* k_reenter larger than zero.
	*/
	// if (k_reenter == 0 && !iskernelp(saved_proc))
	// {
	// 	cause_sig(proc_nr(saved_proc), ep->signum);
	// 	return;
	// }

	/* Exception in system code. This is not supposed to happen. */
	kprintf("\nException #%d: %s\n", exception_type, ep->msg);
	kprintf("k_reenter = %d, ", k_reenter);
	kprintf("process %d (%s), ", proc_nr(saved_proc), saved_proc->p_name);
	kprintf("pc = 0x%X", (unsigned)saved_proc->p_reg.pc);

	panic("exception in a kernel task", NO_NUM);
}
