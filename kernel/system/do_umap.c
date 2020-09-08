/* The kernel call implemented in this file:
 *   m_type:	SYS_UMAP
 *
 * The parameters for this kernel call are:
 *    m5_i1:	CP_SRC_PROC_NR	(process number)
 *    m5_l1:	CP_SRC_ADDR	(virtual address)
 *    m5_l2:	CP_DST_ADDR	(returns physical address)
 *    m5_l3:	CP_NR_BYTES	(size of datastructure)
 */

#include "../system.h"

#if USE_UMAP

/*==========================================================================*
 *				do_umap					    *
 *==========================================================================*/
PUBLIC int do_umap(
    message *m_ptr /* pointer to request message */
)
/* Map virtual address to physical, for non-kernel processes. */
{
	vir_bytes offset = m_ptr->CP_SRC_ADDR;
	int count = m_ptr->CP_NR_BYTES;
	int endpt = (int)m_ptr->CP_SRC_ENDPT;
	int proc_nr;
	phys_bytes phys_addr;

	/* Verify process number. */
	if (endpt == SELF)
		proc_nr = who_p;
	else if (!isokendpt(endpt, &proc_nr))
		return EINVAL;

	phys_addr = umap_local(proc_addr(proc_nr), 0, offset, count);
	//phys_addr = umap_remote(proc_addr(proc_nr), 0, offset, count);

	m_ptr->CP_DST_ADDR = phys_addr;
	return (phys_addr == 0) ? EFAULT : OK;
}

#endif /* USE_UMAP */
