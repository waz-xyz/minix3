#include "syslib.h"

/*===========================================================================*
 *                                sys_umap				     *
 *===========================================================================*/
PUBLIC int sys_umap(
	int proc_nr,			/* process number to do umap for */
	vir_bytes vir_addr,		/* address in bytes */
	vir_bytes bytes,		/* number of bytes to be copied */
	phys_bytes *phys_addr		/* placeholder for result */
)
{
	message m;
	int result;

	m.CP_SRC_ENDPT = proc_nr;
	m.CP_SRC_ADDR = vir_addr;
	m.CP_NR_BYTES = bytes;

	result = _taskcall(SYSTASK, SYS_UMAP, &m);
	if (phys_addr != NULL)
		*phys_addr = m.CP_DST_ADDR;
	return result;
}
