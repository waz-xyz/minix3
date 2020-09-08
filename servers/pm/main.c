/* This file contains the main program of the process manager and some related
 * procedures.  When MINIX starts up, the kernel runs for a little while,
 * initializing itself and its tasks, and then it runs PM and FS.  Both PM
 * and FS initialize themselves as far as they can. PM asks the kernel for
 * all free memory and starts serving requests.
 *
 * The entry points into this file are:
 *   main:	starts PM running
 *   setreply:	set the reply to be sent to process making an PM system call
 */

#include "pm.h"
#include <minix/keymap.h>
#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/endpoint.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <string.h>
#include "mproc.h"
#include "param.h"

#include "../../kernel/const.h"
#include "../../kernel/config.h"
#include "../../kernel/type.h"
#include "../../kernel/proc.h"

FORWARD void get_work(void);
FORWARD void pm_init(void);
FORWARD int get_nice_value(int queue);

/*===========================================================================*
 *				main					     *
 *===========================================================================*/
PUBLIC int main(void)
{
	/* Main routine of the process manager. */
	int result, s, proc_nr;
	struct mproc *rmp;
	sigset_t sigset;

	LED_CONTROL = LED_RED;
	pm_init();	/* initialize process manager tables */

	/* This is PM's main loop-  get work and do it, forever and forever. */
	while (TRUE)
	{
		get_work();	/* wait for an PM system call */

		/* Check for system notifications first. Special cases. */
		if (call_nr == SYN_ALARM)
		{
			pm_expire_timers(m_in.NOTIFY_TIMESTAMP);
			result = SUSPEND;	/* don't reply */
		}
		else if (call_nr == SYS_SIG)	/* signals pending */
		{
			sigset = m_in.NOTIFY_ARG;
			if (sigismember(&sigset, SIGKSIG))
			{
				(void)ksig_pending();
			}
			result = SUSPEND;	/* don't reply */
		}
		/* Else, if the system call number is valid, perform the call. */
		else if ((unsigned)call_nr >= NCALLS)
		{
			result = ENOSYS;
		}
		else
		{
			result = (*call_vec[call_nr])();
		}

		/* Send the results back to the user to indicate completion. */
		if (result != SUSPEND)
			setreply(who_p, result);

		swap_in();	/* maybe a process can be swapped in? */

		/* Send out all pending reply messages, including the answer to
		 * the call just made above.  The processes must not be swapped out.
		 */
		for (proc_nr = 0, rmp = mproc; proc_nr < NR_PROCS; proc_nr++, rmp++)
		{
			/* In the meantime, the process may have been killed by a
			 * signal (e.g. if a lethal pending signal was unblocked)
			 * without the PM realizing it. If the slot is no longer in
			 * use or just a zombie, don't try to reply.
			 */
			if ((rmp->mp_flags & (REPLY | ONSWAP | IN_USE | ZOMBIE)) == (REPLY | IN_USE))
			{
				if ((s = send(rmp->mp_endpoint, &rmp->mp_reply)) != OK)
				{
					printf("PM can't reply to %d (%s)\n",
					       rmp->mp_endpoint, rmp->mp_name);
					panic(__FILE__, "PM can't reply", NO_NUM);
				}
				rmp->mp_flags &= ~REPLY;
			}
		}
	}
	return OK;
}

/*===========================================================================*
 *				get_work				     *
 *===========================================================================*/
PRIVATE void get_work(void)
{
	/* Wait for the next message and extract useful information from it. */
	if (receive(ANY, &m_in) != OK)
		panic(__FILE__, "PM receive error", NO_NUM);
	who_e = m_in.m_source;	/* who sent the message */
	if (pm_isokendpt(who_e, &who_p) != OK)
		panic(__FILE__, "PM got message from invalid endpoint", who_e);
	call_nr = m_in.m_type;	/* system call number */

	/* Process slot of caller. Misuse PM's own process slot if the kernel is
	 * calling. This can happen in case of synchronous alarms (CLOCK) or or 
	 * event like pending kernel signals (SYSTEM).
	 */
	mp = &mproc[who_p < 0 ? PM_PROC_NR : who_p];
	if (who_p >= 0 && mp->mp_endpoint != who_e)
	{
		panic(__FILE__, "PM endpoint number out of sync with source",
		      mp->mp_endpoint);
	}
}

/*===========================================================================*
 *				setreply				     *
 *===========================================================================*/
PUBLIC void setreply(
	int proc_nr,			/* process to reply to */
	int result			/* result of call (usually OK or error #) */
)
/* Fill in a reply message to be sent later to a user process.  System calls
 * may occasionally fill in other fields, this is only for the main return
 * value, and for setting the "must send reply" flag.
 */
{
	register struct mproc *rmp = &mproc[proc_nr];

	if (proc_nr < 0 || proc_nr >= NR_PROCS)
		panic(__FILE__, "setreply arg out of range", proc_nr);

	rmp->mp_reply.reply_res = result;
	rmp->mp_flags |= REPLY; /* reply pending */

	if (rmp->mp_flags & ONSWAP)
		swap_inqueue(rmp); /* must swap this process back in */
}

/*===========================================================================*
 *				pm_init					     *
 *===========================================================================*/
PRIVATE void pm_init(void)
/* Initialize the process manager. 
 * Memory use info is collected from the boot monitor, the kernel, and
 * all processes compiled into the system image.
 */
{
	int s;
	static struct boot_image image[NR_BOOT_PROCS];
	register struct boot_image *ip;
	static char core_sigs[] = {SIGQUIT, SIGILL, SIGTRAP, SIGABRT,
				   SIGEMT, SIGFPE, SIGUSR1, SIGSEGV, SIGUSR2};
	static char ign_sigs[] = {SIGCHLD, SIGWINCH, SIGCONT};
	static char mess_sigs[] = {SIGTERM, SIGHUP, SIGABRT, SIGQUIT};
	register struct mproc *rmp;
	register int i;
	register char *sig_ptr;
	phys_bytes total_mem, minix_mem, free_mem;
	message mess;
	struct mem_map mem_map[NR_LOCAL_SEGS];

	/* Initialize process table, including timers. */
	for (rmp = &mproc[0]; rmp < &mproc[NR_PROCS]; rmp++)
	{
		tmr_inittimer(&rmp->mp_timer);
	}
	
	/* Build the set of signals which cause core dumps, and the set of signals
	 * that are by default ignored.
	 */
	sigemptyset(&core_sset);
	for (sig_ptr = core_sigs; sig_ptr < core_sigs + sizeof(core_sigs); sig_ptr++)
		sigaddset(&core_sset, *sig_ptr);
	sigemptyset(&ign_sset);
	for (sig_ptr = ign_sigs; sig_ptr < ign_sigs + sizeof(ign_sigs); sig_ptr++)
		sigaddset(&ign_sset, *sig_ptr);

	/* Obtain a copy of the boot monitor parameters and the kernel info struct.  
	 * Parse the list of free memory chunks. This list is what the boot monitor 
	 * reported, but it must be corrected for the kernel and system processes.
	 */
	if ((s = sys_getmonparams(monitor_params, sizeof(monitor_params))) != OK)
		panic(__FILE__, "get monitor params failed", s);
	if ((s = sys_getkinfo(&kinfo)) != OK)
		panic(__FILE__, "get kernel info failed", s);

	/* Get the memory map of the kernel to see how much memory it uses. */
	if ((s = get_mem_map(SYSTASK, mem_map)) != OK)
		panic(__FILE__, "couldn't get memory map of SYSTASK", s);
	minix_mem = (mem_map[S].mem_phys + mem_map[S].mem_len) - mem_map[T].mem_phys;

	/* Initialize PM's process table. Request a copy of the system image table 
	 * that is defined at the kernel level to see which slots to fill in.
	 */
	if (OK != (s = sys_getimage(image)))
		panic(__FILE__, "couldn't get image table: %d\n", s);
	procs_in_use = 0;			/* start populating table */
	printf("Building process table:");	/* show what's happening */
	for (ip = &image[0]; ip < &image[NR_BOOT_PROCS]; ip++)
	{
		if (ip->proc_nr < 0)		/* task have negative nrs */
		{
			continue;
		}
		procs_in_use += 1;		/* found user process */

		/* Set process details found in the image table. */
		rmp = &mproc[ip->proc_nr];
		strncpy(rmp->mp_name, ip->proc_name, PROC_NAME_LEN);
		rmp->mp_parent = RS_PROC_NR;
		rmp->mp_nice = get_nice_value(ip->priority);
		sigemptyset(&rmp->mp_sig2mess);
		sigemptyset(&rmp->mp_ignore);
		sigemptyset(&rmp->mp_sigmask);
		sigemptyset(&rmp->mp_catch);
		if (ip->proc_nr == INIT_PROC_NR) /* user process */
		{
			rmp->mp_procgrp = rmp->mp_pid = INIT_PID;
			rmp->mp_flags |= IN_USE;
		}
		else /* system process */
		{
			rmp->mp_pid = get_free_pid();
			rmp->mp_flags |= IN_USE | DONT_SWAP | PRIV_PROC;
			for (sig_ptr = mess_sigs;
				sig_ptr < mess_sigs + sizeof(mess_sigs);
				sig_ptr++)
			{
				sigaddset(&rmp->mp_sig2mess, *sig_ptr);
			}
		}

		/* Get kernel endpoint identifier. */
		rmp->mp_endpoint = ip->endpoint;

		/* Get memory map for this process from the kernel. */
		if ((s = get_mem_map(ip->proc_nr, rmp->mp_seg)) != OK)
			panic(__FILE__, "couldn't get process entry", s);
		if (rmp->mp_seg[T].mem_len != 0)
			rmp->mp_flags |= SEPARATE;
		minix_mem += rmp->mp_seg[S].mem_phys +
				rmp->mp_seg[S].mem_len - rmp->mp_seg[T].mem_phys;

		/* Tell FS about this system process. */
		mess.PR_SLOT = ip->proc_nr;
		mess.PR_PID = rmp->mp_pid;
		mess.PR_ENDPT = rmp->mp_endpoint;
		if (OK != (s = send(FS_PROC_NR, &mess)))
			panic(__FILE__, "can't sync up with FS", s);
		printf(" %s", ip->proc_name);		/* display process name */
	}
	printf(".\n");	/* last process done */

	/* Override some details. INIT, PM, FS and RS are somewhat special. */
	mproc[PM_PROC_NR].mp_pid = PM_PID;		/* PM has magic pid */
	mproc[RS_PROC_NR].mp_parent = INIT_PROC_NR;	/* INIT is root */
	sigfillset(&mproc[PM_PROC_NR].mp_ignore);	/* guard against signals */

	/* Tell FS that no more system processes follow and synchronize. */
	mess.PR_ENDPT = NONE;
	if (sendrec(FS_PROC_NR, &mess) != OK || mess.m_type != OK)
		panic(__FILE__, "can't sync up with FS", NO_NUM);
// LED_CONTROL = LED_GREEN; LED_CONTROL = LED_BLUE;
#if ENABLE_BOOTDEV
	/* Possibly we must correct the memory chunks for the boot device. */
	if (kinfo.bootdev_size > 0)
	{
		mem_map[T].mem_phys = kinfo.bootdev_base >> CLICK_SHIFT;
		mem_map[T].mem_len = 0;
		mem_map[D].mem_len = (kinfo.bootdev_size + CLICK_SIZE - 1) >> CLICK_SHIFT;
		patch_mem_chunks(mem_chunks, mem_map);
	}
#endif /* ENABLE_BOOTDEV */

	/* Initialize tables to all physical memory and print memory information. */
	printf("Physical memory:");
	free_mem = kinfo.free_mem;
	total_mem = minix_mem + free_mem;
	printf(" total %u bytes,", total_mem);
	printf(" system %u bytes,", minix_mem);
	printf(" free %u bytes.\n", free_mem);
}

/*===========================================================================*
 *				get_nice_value				     *
 *===========================================================================*/
PRIVATE int get_nice_value(
	int queue			/* store mem chunks here */
)
/* Processes in the boot image have a priority assigned. The PM doesn't know
 * about priorities, but uses 'nice' values instead. The priority is between 
 * MIN_USER_Q and MAX_USER_Q. We have to scale between PRIO_MIN and PRIO_MAX.
 */
{
	int nice_val = (queue - USER_Q) * (PRIO_MAX - PRIO_MIN + 1) /
		       (MIN_USER_Q - MAX_USER_Q + 1);
	if (nice_val > PRIO_MAX)
		nice_val = PRIO_MAX;	/* shouldn't happen */
	if (nice_val < PRIO_MIN)
		nice_val = PRIO_MIN;	/* shouldn't happen */
	return nice_val;
}
