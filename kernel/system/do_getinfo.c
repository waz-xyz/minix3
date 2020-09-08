/* The kernel call implemented in this file:
 *   m_type:	SYS_GETINFO
 *
 * The parameters for this kernel call are:
 *    m1_i3:	I_REQUEST	(what info to get)	
 *    m1_p1:	I_VAL_PTR 	(where to put it)	
 *    m1_i1:	I_VAL_LEN 	(maximum length expected, optional)	
 *    m1_p2:	I_VAL_PTR2	(second, optional pointer)	
 *    m1_i2:	I_VAL_LEN2_E	(second length or process nr)	
 */

#include "../system.h"
#include <string.h>

static unsigned long bios_buf[1024];	/* 4K, what about alignment */
static vir_bytes bios_buf_vir, bios_buf_len;

#if USE_GETINFO

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
PUBLIC int do_getinfo(
	message *m_ptr			/* pointer to request message */
)
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
{
	size_t length;
	void *src_addr; 
	void *dst_addr; 
	int proc_nr, nr_e, nr;

	/* Set source address and length based on request type. */
	switch (m_ptr->I_REQUEST)
	{	
		case GET_MACHINE:
		{
			length = sizeof(struct machine);
			src_addr = &machine;
			break;
		}
		case GET_KINFO:
		{
			length = sizeof(struct kinfo);
			src_addr = &kinfo;
			break;
		}
		case GET_LOADINFO:
		{
			length = sizeof(struct loadinfo);
			src_addr = &kloadinfo;
			break;
		}
		case GET_IMAGE:
		{
			length = sizeof(struct boot_image) * NR_BOOT_PROCS;
			src_addr = image;
			break;
		}
		case GET_IRQHOOKS:
		{
			length = sizeof(struct irq_hook) * NR_IRQ_HOOKS;
			src_addr = irq_hooks;
			break;
		}
		case GET_SCHEDINFO:
		{
			/* This is slightly complicated because we need two data structures
			* at once, otherwise the scheduling information may be incorrect.
			* Copy the queue heads and fall through to copy the process table. 
			*/
			length = sizeof(struct proc *) * NR_SCHED_QUEUES;
			src_addr = rdy_head;
			okendpt(m_ptr->m_source, &proc_nr);
			dst_addr = validate_user_ptr(proc_nr, m_ptr->I_VAL_PTR2, length, PTR_WRITABLE); 
			if (src_addr == 0 || dst_addr == 0)
				return EFAULT;
			memcpy(dst_addr, src_addr, length);
			/* fall through */
		}
		case GET_PROCTAB:
		{
			length = sizeof(struct proc) * (NR_PROCS + NR_TASKS);
			src_addr = proc;
			break;
		}
		case GET_PRIVTAB:
		{
			length = sizeof(struct priv) * (NR_SYS_PROCS);
			src_addr = priv;
			break;
		}
		case GET_PROC:
		{
			nr_e = (m_ptr->I_VAL_LEN2_E == SELF) ? m_ptr->m_source : m_ptr->I_VAL_LEN2_E;
			if(!isokendpt(nr_e, &nr))		/* validate request */
				return EINVAL;
			length = sizeof(struct proc);
			src_addr = proc_addr(nr);
			break;
		}
		case GET_MONPARAMS:
		{
			src_addr = (void*) kinfo.params_base;
			length = kinfo.params_size;
			break;
		}
		case GET_RANDOMNESS:
		{		
			static struct randomness copy;		/* copy to keep counters */
			int i;

			copy = krandom;
			for (i = 0; i < RANDOM_SOURCES; i++)
			{
				krandom.bin[i].r_size = 0;	/* invalidate random data */
				krandom.bin[i].r_next = 0;
			}
			length = sizeof(struct randomness);
			src_addr = &copy;
			break;
		}
		case GET_KMESSAGES:
		{
			length = sizeof(struct kmessages);
			src_addr = &kmess;
			break;
		}
#if DEBUG_TIME_LOCKS
		case GET_LOCKTIMING:
		{
			length = sizeof(timingdata);
			src_phys = timingdata;
			break;
		}
#endif
		case GET_BIOSBUFFER:
		{
			bios_buf_vir = (vir_bytes)bios_buf;
			bios_buf_len = sizeof(bios_buf);

			length = sizeof(bios_buf_len);
			src_addr = &bios_buf_len;
			if (length != m_ptr->I_VAL_LEN2_E)
				return EINVAL;
			if (!isokendpt(m_ptr->m_source, &proc_nr))
				panic("bogus source", m_ptr->m_source);
			dst_addr = validate_user_ptr(proc_nr, m_ptr->I_VAL_PTR2, length, PTR_WRITABLE); 
			if (src_addr == 0 || dst_addr == 0)
				return EFAULT;
			memcpy(dst_addr, src_addr, length);
			length = sizeof(bios_buf_vir);
			src_addr = &bios_buf_vir;
			break;
		}
		case GET_IRQACTIDS:
		{
			length = sizeof(irq_actids);
			src_addr = irq_actids;
			break;
		}
		default:
			return EINVAL;
	}

	/* Try to make the actual copy for the requested data. */
	if (m_ptr->I_VAL_LEN > 0 && length > m_ptr->I_VAL_LEN)
		return E2BIG;
	if (!isokendpt(m_ptr->m_source, &proc_nr)) 
		panic("bogus source", m_ptr->m_source);
	dst_addr = validate_user_ptr(proc_nr, m_ptr->I_VAL_PTR, length, PTR_WRITABLE); 
	if (src_addr == 0 || dst_addr == 0)
		return EFAULT;
	memcpy(dst_addr, src_addr, length);
	
	return OK;
}

#endif /* USE_GETINFO */
