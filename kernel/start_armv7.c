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
}