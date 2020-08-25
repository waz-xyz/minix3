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
	struct ex_s *ep;
	struct proc *saved_proc;
	/* For SVC exceptions: */
	int call_nr;
	int src_dst;
	message *m_ptr;
	long bit_map;

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
	// if (k_reenter == 0 && 
	if (k_reenter == 0)
	{
		switch (exception_type)
		{
			case SUPERVISOR_CALL_EXCEPTION:
				call_nr = saved_proc->p_reg.r3;
				src_dst = saved_proc->p_reg.r0;
				m_ptr = (message*)saved_proc->p_reg.r1;
				bit_map = saved_proc->p_reg.r2;
				sys_call(call_nr, src_dst, m_ptr, bit_map);
				return;
			default:
				// if (!iskernelp(saved_proc))
				// {
				// 	cause_sig(proc_nr(saved_proc), ep->signum);
				// 	return;
				// }
				break;
		}
	}

	/* Exception in system code. This is not supposed to happen. */
	kprintf("\nException #%d: %s\n", exception_type, ep->msg);
	kprintf("k_reenter = %d\n", k_reenter);
	if (saved_proc != NULL)
	{
		kprintf("process %d (%s)\n", proc_nr(saved_proc), saved_proc->p_name);
		kprintf("pc = 0x%08X\n", saved_proc->p_reg.pc);
		kprintf("psw = 0x%08X\n", saved_proc->p_reg.psw);
		kprintf("sp = 0x%08X\n", saved_proc->p_reg.sp);
		if (exception_type == UNDEFINED_INSTRUCTION_EXCEPTION)
		{
			uint32_t *p = (uint32_t*)saved_proc->p_reg.pc;
			kprintf("instruction at pc = 0x%08X\n", *p);
			kprintf("instruction at pc-4 = 0x%08X\n", *(p-1));
		}
		else if (exception_type == PREFETCH_ABORT_EXCEPTION)
		{
			kprintf("ifsr = 0x%08X\n", read_system_register(READ_IFSR));
			kprintf("ifar = 0x%08X\n", read_system_register(READ_IFAR));
		}
		else if (exception_type == DATA_ABORT_EXCEPTION)
		{
			kprintf("dfsr = 0x%08X\n", read_system_register(READ_DFSR));
			kprintf("dfar = 0x%08X\n", read_system_register(READ_DFAR));
		}
	}
	else
	{
		kprintf("Abnormal condition: proc_ptr == NULL\n");
	}

	panic("exception in a kernel task", NO_NUM);
}
