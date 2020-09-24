/* The system call implemented in this file:
 *   m_type:	SYS_VM_MAP
 *
 * The parameters for this system call are:
 *    m4_l1:	Process that requests map (VM_MAP_ENDPT)
 *    m4_l2:	Map (TRUE) or unmap (FALSE) (VM_MAP_MAPUNMAP)
 *    m4_l3:	Base address (VM_MAP_BASE)
 *    m4_l4:	Size  (VM_MAP_SIZE)
 *    m4_l5:	Memory address  (VM_MAP_ADDR)
 */
#include "../system.h"

#include <sys/vm.h>

/*===========================================================================*
 *				do_vm_map				     *
 *===========================================================================*/
PUBLIC int do_vm_map(
	message *m_ptr			/* pointer to request message */
)
{
	int proc_nr, do_map;
	phys_bytes base, size;
	uint32_t offset;
	struct proc *pp;
	int ret;

	if (m_ptr->VM_MAP_ENDPT == SELF)
	{
		proc_nr = who_p;
	}
	else if (!isokendpt(m_ptr->VM_MAP_ENDPT, &proc_nr))
	{
		return EINVAL;
	}

	do_map = m_ptr->VM_MAP_MAPUNMAP;
	base = m_ptr->VM_MAP_BASE;
	size = m_ptr->VM_MAP_SIZE;
	pp = proc_addr(proc_nr);
	
	if (do_map)
	{
		pp->p_misc_flags |= MF_VM;
		ret = map_physical_range(pp, base, size, &offset);
		if (ret != OK) return ret;
		m_ptr->VM_MAP_ADDR = offset;
		// kprintf("do_vm_map: New offset = 0x%08X\n", offset);
	}
	else
	{
		offset = m_ptr->VM_MAP_ADDR;
		unmap_physical_range(pp, base, size, offset);
	}
	invalidate_system_structures(INVALIDATE_TLB);
	print_mmu_tables(pp);

	return OK;
}
