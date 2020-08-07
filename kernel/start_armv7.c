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
        
}