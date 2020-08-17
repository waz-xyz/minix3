#include "kernel.h"
#include "protect.h"
#include "proc.h"
#include <stdlib.h>
#include <string.h>
#include <elf.h>

#define OFFSET_16MB     0x1000000

void allocate_page_tables(struct proc *pr)
{

}

uint32_t allocate_task_stack(void)
{
        return 0;
}

void *get_header_from_image(int progindex)
{
        if (progindex <= 0 || progindex >= number_of_programs)
                return NULL;
        
        return (void*) (KERNEL_RAW_ACCESS_BASE + programs_locations[progindex]);
}

uint32_t vir2phys(void *address)
{
        uint32_t addr = (uint32_t) address;

        if (addr >= KERNEL_RAW_ACCESS_BASE)
        {
                return addr - KERNEL_RAW_ACCESS_BASE + KERNEL_PHYSICAL_BASE;
        }
        else
        {
                return addr - KERNEL_VIRTUAL_BASE + KERNEL_PHYSICAL_BASE;
        }       
}

void *phys2vir(uint32_t address)
{
        if (KERNEL_PHYSICAL_BASE <= address && address < (KERNEL_PHYSICAL_BASE + OFFSET_16MB))
        {
                return (void*)(address - KERNEL_PHYSICAL_BASE + KERNEL_RAW_ACCESS_BASE);
        }
        else
        {
                return NULL;
        }       
}