/* This file contains the C startup code for Minix on Armv7 processors.
 * It cooperates with mpx.s to set up a good environment for main().
 */

#include "kernel.h"
#include "protect.h"
#include "proc.h"
#include <stdlib.h>
#include <string.h>

/*===========================================================================*
 *				cstart					     *
 *===========================================================================*/
PUBLIC void cstart(void)
{
	extern void *__text_start, *__text_end;
	extern void *__data_start, *__data_end;
	extern void *__end;
	extern const char *__bss_start, *__bss_end;

	serial_init();
	serial_puts("Starting kernel...");

	const char *p =  __bss_start;
	const char *end = __bss_end;
	while (p < end) {
		if (*p != 0){
			serial_puts("Bad initialized BSS at address");
			serial_printHex((uint32_t)p);
			break;
		}
		p++;
	}
	if (p >= end) {
		serial_puts("All BSS looks good");
	}

	/* Record where the kernel is. */
	kinfo.code_base = (phys_bytes) __text_start;
	kinfo.code_size = (phys_bytes) __text_end;	/* size of code segment */
	kinfo.data_base = (phys_bytes) __data_start;
	kinfo.data_size = (phys_bytes) __data_end;	/* size of data segment */

	/* Record miscellaneous information for user-space servers. */
	kinfo.nr_procs = NR_PROCS;
	kinfo.nr_tasks = NR_TASKS;
	strncpy(kinfo.release, OS_RELEASE, sizeof(kinfo.release));
	kinfo.release[sizeof(kinfo.release)-1] = '\0';
	strncpy(kinfo.version, OS_VERSION, sizeof(kinfo.version));
	kinfo.version[sizeof(kinfo.version)-1] = '\0';
	kinfo.proc_addr = (vir_bytes) proc;
	kinfo.kmem_base = kinfo.code_base;
	kinfo.kmem_size = (phys_bytes) (__end - kinfo.kmem_base);

	/* Load average data initialization. */
	kloadinfo.proc_last_slot = 0;
	for(int h = 0; h < _LOAD_HISTORY; h++)
		kloadinfo.proc_load_history[h] = 0;
	
	main();
}
