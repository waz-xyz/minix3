#include "kernel.h"
#include "protect.h"
#include "proc.h"
#include <stdlib.h>
#include <string.h>
#include <elf.h>

#define OFFSET_16MB     0x1000000

static uint32_t *kernel_1st_level_tt = NULL;
static uint32_t *kernel_page_table = NULL;
static uint32_t kernel_stack_start, kernel_stack_end;
static uint32_t kernel_mmu_tables_start, kernel_mmu_tables_end;
static uint32_t next_free_pt_location = 0;
static uint32_t next_free_user_tt_location = 0;
static uint32_t next_free_smallpage_location = 0;
static uint32_t user_tt_start, user_tt_end;

static void print_1st_level_table(uint32_t *table, int is_user);
static void print_page_table(uint32_t *table);
static void set_exception_vector_table(void);
static void SetPageTableDescriptor(uint32_t *tt, int index, uint32_t base, int domain);
static void SetSmallPageDescriptor(uint32_t *pt, int index, uint32_t base, int ap, int tex, int c, int b);

void init_mmu_module(void)
{
	uint32_t pos;
	
	/* Initialize to the final location of the boot image in virtual memory,
	 * aligned to next free small page */
	kernel_stack_start = pos = KERNEL_RAW_ACCESS_BASE + KERNEL_PHYSICAL_BASE + ALIGN_TO_SMALL_PAGE(end_of_image);
	/* Add space for kernel stack */
	kernel_stack_end = pos = pos + KERNEL_STACK_SIZE;
	/* Align next position to a 1st-level table */
	kernel_mmu_tables_start = pos = ALIGN_TO_POWER_OF_2(kernel_stack_end, KERNEL_FIRST_LEVEL_TT_SIZE);
	kernel_1st_level_tt = (uint32_t*) kernel_mmu_tables_start;
	kernel_page_table = (uint32_t*) ALIGN_TO_SMALL_PAGE(pos + KERNEL_FIRST_LEVEL_TT_SIZE);
	/* Add space for kernel's MMU tables */
	kernel_mmu_tables_end = pos = pos + KERNEL_MMU_TABLES_SIZE;
	/* Align to next possible user 1st-level table */
	user_tt_start = next_free_user_tt_location = ALIGN_TO_POWER_OF_2(pos, USER_FIRST_LEVEL_TT_SIZE);
	user_tt_end = user_tt_start + (number_of_programs - 1) * USER_FIRST_LEVEL_TT_SIZE;
	/* Find some holes to use as a free location for page tables */
	if (kernel_mmu_tables_start - ALIGN_TO_PAGE_TABLE(kernel_stack_end) >= PAGE_TABLE_SIZE)
	{
		next_free_pt_location = ALIGN_TO_PAGE_TABLE(kernel_stack_end);
	}
	else if (user_tt_start - ALIGN_TO_PAGE_TABLE(kernel_mmu_tables_end) >= PAGE_TABLE_SIZE)
	{
		next_free_pt_location = ALIGN_TO_PAGE_TABLE(kernel_mmu_tables_end);
	}
	else
	{
		next_free_pt_location = ALIGN_TO_PAGE_TABLE(user_tt_end);
	}

	set_exception_vector_table();

	kprintf("next_free_pt_location: 0x%08X\n", next_free_pt_location);
	kprintf("user_tt_start: 0x%08X\n", user_tt_start);
	kprintf("user_tt_end: 0x%08X\n", user_tt_end);
	kprintf("kernel_mmu_tables_start: 0x%08X\n", kernel_mmu_tables_start);
	kprintf("kernel_page_table: 0x%08X\n", kernel_page_table);
	kprintf("Kernel's first-level table:\n");
	print_1st_level_table(kernel_1st_level_tt, 0);
	// kprintf("Kernel's page table:\n");
	// print_page_table(kernel_page_table);
}

void set_exception_vector_table(void)
{
	uint32_t *pt = kernel_page_table + PAGE_TABLE_SIZE/4;
	uint32_t base = vir2phys(&exception_vector_start);
	kprintf("exception_vector_start physical address = 0x%08X\n", base);
	SetSmallPageDescriptor(pt, 0xF0, base, 5, 5, 0, 1);

	uint32_t *v = (uint32_t*)0xFFFF0000;
	for (int i = 0; i < 15; i++)
	{
		kprintf("%08X: 0x%08X\n", &v[i], v[i]);
	}
}

static void print_1st_level_table(uint32_t *table, int is_user)
{
	int limit = is_user ? USER_FIRST_LEVEL_TT_NOF_ENTRIES : KERNEL_FIRST_LEVEL_TT_NOF_ENTRIES;

	kprintf("Index\tEntry\t\tType\tBase\t\tDomain\tS\tnG\tTEX[2:0]\tC\tB\tAP[2:0]\n"
		"------------------------------------------------------------------------"
		"---------------------------------------\n");
	for (int i = 0; i < limit; i++)
	{
		uint32_t entry;
		const char *type;
		int id;
		uint32_t base_address;
		int domain;
		int s, nG, c, b, tex2, tex1, tex0, ap2, ap1, ap0;

		entry = table[i];
		id = entry & 0x3;
		if (id == 0)
		{
			continue;
		}
		else if (id == 1)
		{
			type = "PT";
			base_address = entry & ~0x3FF;
			domain = (entry >> 5) & 0xF;
			kprintf("#%03X\t0x%08X\t%s\t0x%08X\t%d", i, entry, type, base_address, domain);
		}
		else if (id == 2)
		{
			int isSuper = ((entry >> 18) & 1) == 1;
			type = isSuper ? "SS" : "SE";
			base_address = isSuper ? (entry & ~0xFFFFFF) : (entry & ~0xFFFFF);
			kprintf("#%03X\t0x%08X\t%s\t0x%08X\t", i, entry, type, base_address);
			if (isSuper)
			{
				kprintf("\t");
			}
			else
			{
				domain = (entry >> 5) & 0xF;
				kprintf("%d\t", domain);
			}
			s = (entry >> 16) & 1;
			nG = (entry >> 17) & 1;
			b = (entry >> 2) & 1;
			c = (entry >> 3) & 1;
			ap0 = (entry >> 10) & 1;
			ap1 = (entry >> 11) & 1;
			ap2 = (entry >> 15) & 1;
			tex0 = (entry >> 12) & 1;
			tex1 = (entry >> 13) & 1;
			tex2 = (entry >> 14) & 1;
			kprintf("%d\t%d\t%d %d %d\t\t%d\t%d\t%d %d %d",
				s, nG, tex2, tex1, tex0, c, b, ap2, ap1, ap0);
		}
		else /* if (id == 3) */
		{
			type = "RE";
			kprintf("#%03X\t0x%08X\t%s", i, entry, type);
		}
		kprintf("\n");
	}
}

static void print_page_table(uint32_t *table)
{
	kprintf("Index\tEntry\t\tType\tBase\t\tS\tnG\tTEX[2:0]\tC\tB\tAP[2:0]\t\tXN\n"
		"----------------------------------------------------------------------"
		"--------------------------------------------\n");
	for (int i = 0; i < PAGE_TABLE_NOF_ENTRIES; i++)
	{
		uint32_t entry;
		const char *type;
		int id;
		uint32_t base_address;
		int c, b, s, ap2, ap1, ap0, tex2, tex1, tex0, nG, xn;

		entry = table[i];
		id = entry & 0x3;
		if (id == 0)
		{
			continue;
		}
		else if (id == 1)
		{
			type = "LP";
			base_address = entry & ~0xFFFFU;
			tex0 = (entry >> 12) & 1;
			tex1 = (entry >> 13) & 1;
			tex2 = (entry >> 14) & 1;
			xn = (entry >> 15) & 1;
		}
		else
		{
			type = "SP";
			base_address = entry & ~0xFFFU;
			tex0 = (entry >> 6) & 1;
			tex1 = (entry >> 7) & 1;
			tex2 = (entry >> 8) & 1;
			xn = entry & 1;
		}
		b = (entry >> 2) & 1;
		c = (entry >> 3) & 1;
		ap0 = (entry >> 4) & 1;
		ap1 = (entry >> 5) & 1;
		ap2 = (entry >> 9) & 1;
		s = (entry >> 10) & 1;
		nG = (entry >> 11) & 1;
		kprintf("#%02X\t0x%08X\t%s\t0x%08X\t%d\t%d\t%d %d %d\t\t%d\t%d\t%d %d %d\t\t%d\n",
			i, entry, type, base_address, s, nG, tex2, tex1, tex0, c, b, ap2, ap1, ap0, xn);
	}
}

static void print_mmu_tables(uint32_t *table, int is_user)
{
	int limit = is_user ? USER_FIRST_LEVEL_TT_NOF_ENTRIES : KERNEL_FIRST_LEVEL_TT_NOF_ENTRIES;

	print_1st_level_table(table, is_user);
	for (int i = 0; i < limit; i++)
	{
		uint32_t entry = table[i];
		if ((entry & 3) == 1)
		{
			uint32_t *base = phys2vir(entry & ~0x3FF);
			kprintf("~~~ Page table for index #%03X ~~~\n", i);
			print_page_table(base);
		}
	}
}

static void SetPageTableDescriptor(uint32_t *tt, int index, uint32_t base, int domain)
{
	tt[index] = (base & (~0x3FF)) | (domain << 5) | 1;
}

static void SetSmallPageDescriptor(uint32_t *pt, int index, uint32_t base, int ap, int tex, int c, int b)
{
	int ap2, ap10;
	ap2 = (ap >> 2) & 1;
	ap10 = ap & 3;
	pt[index] = (base & (~0xFFF)) | (ap2 << 9) | (tex << 6) | (ap10 << 4) | (c << 3) | (b << 2) | 2;
}

static void *GetNextFreeUserTTLocation(void)
{
	uint32_t cur = next_free_user_tt_location;
	next_free_user_tt_location += USER_FIRST_LEVEL_TT_SIZE;
	memset((void*)cur, 0, USER_FIRST_LEVEL_TT_SIZE);
	return (void*) cur;
}

static uint32_t GetNextFreePageTableLocation(void)
{
	uint32_t pt_start;
	pt_start = next_free_pt_location;
	if (kernel_mmu_tables_start <= pt_start && pt_start < kernel_mmu_tables_end)
	{
		pt_start = ALIGN_TO_PAGE_TABLE(kernel_mmu_tables_end);
	}
	if (user_tt_start <= pt_start && pt_start < user_tt_end)
	{
		pt_start = ALIGN_TO_PAGE_TABLE(user_tt_end);
	}
	next_free_pt_location = pt_start + PAGE_TABLE_SIZE;
	memset((void*)pt_start, 0, PAGE_TABLE_SIZE);
	return vir2phys((void*) pt_start);
}

static uint32_t GetNextFreeSmallPageLocation(void)
{
	uint32_t sp_start;
	if (next_free_smallpage_location == 0)
	{
		next_free_smallpage_location = ALIGN_TO_SMALL_PAGE(next_free_pt_location);
	}
	sp_start = next_free_smallpage_location;
	next_free_smallpage_location += SMALL_PAGE_SIZE;
	return vir2phys((void*) sp_start);
}

void allocate_page_tables(struct proc *pr)
{
	uint32_t *tt;
	uint32_t virt_start, virt_end;
	struct mem_map *mm;
	
	tt = GetNextFreeUserTTLocation();
	pr->p_ttbase = vir2phys(tt);
	for (int seg = T; seg <= S; seg++)
	{
		mm = &(pr->p_memmap[seg]);
		virt_start = mm->mem_vir;
		virt_end = ALIGN_TO_SMALL_PAGE(virt_start + mm->mem_len);
		for (uint32_t vloc = virt_start; vloc < virt_end; vloc += SMALL_PAGE_SIZE)
		{
			int inx = (vloc >> 20) & 0xFFFU;
			if (tt[inx] == 0)
			{
				SetPageTableDescriptor(tt, inx, GetNextFreePageTableLocation(), 0);
			}
		}
	}
}

void allocate_pages(struct proc *pr)
{
	uint32_t *tt, *pt;
	uint32_t virt_start, virt_end, paddr, paddr_end;
	struct mem_map *mm;
	int ap;

	tt = phys2vir(pr->p_ttbase);
	for (int seg = T; seg <= S; seg++)
	{
		ap = (seg == T) ? 2 : 3;
		mm = &(pr->p_memmap[seg]);
		paddr = mm->mem_phys;
		paddr_end = ALIGN_TO_SMALL_PAGE(mm->mem_phys + mm->mem_plen);
		virt_start = mm->mem_vir;
		virt_end = ALIGN_TO_SMALL_PAGE(virt_start + mm->mem_len);
		for (uint32_t vloc = virt_start; vloc < virt_end; vloc += SMALL_PAGE_SIZE)
		{
			uint32_t base;
			int inx = (vloc >> 20) & 0xFFFU;
			uint32_t entry = tt[inx];
			pt = phys2vir(entry & ~0x3FFU);
			inx = (vloc >> 12) & 0xFFU;
			entry = pt[inx]; 
			if (paddr < paddr_end)
			{	
				base = paddr;
				paddr += SMALL_PAGE_SIZE;
			}
			else
			{
				base = GetNextFreeSmallPageLocation();
			}
			SetSmallPageDescriptor(pt, inx, base, ap, 5, 0, 1);
		}
	}

	// kprintf("MMU tables for %s:\n", pr->p_name);
	// print_mmu_tables(tt, 1);

	if (pr->p_nr == 4)
	{
		kprintf("Kernel page table #1:\n");
		print_page_table(kernel_page_table);
		kprintf("Kernel page table #2:\n");
		print_page_table(kernel_page_table + PAGE_TABLE_SIZE/4);
	}
}

uint32_t allocate_task_stack(void)
{
	int i;
	uint32_t vaddr;

	/* Find the lowest 2 empty entries in the kernel's page table.
	 * One for a guard page and another for the task stack. */
	for (i = 0; i < PAGE_TABLE_NOF_ENTRIES-1; i++)
	{
		if ((kernel_page_table[i] & 3) == 0 && 
		    (kernel_page_table[i+1] & 3) == 0)
		{
			break;
		}
	}
	i++;	/* Skip guard page */
	SetSmallPageDescriptor((uint32_t*)kernel_page_table, i, GetNextFreeSmallPageLocation(), 1, 5, 0, 1);
	vaddr = KERNEL_VIRTUAL_BASE + (i << 12);
	kprintf("New task stack pointer: 0x%08X\n", vaddr + SMALL_PAGE_SIZE);
	return vaddr + SMALL_PAGE_SIZE;
}

void *get_header_from_image(int progindex)
{
	if (progindex <= 0 || progindex >= number_of_programs)
		return NULL;
	
	return (void*) (KERNEL_RAW_ACCESS_BASE + KERNEL_PHYSICAL_BASE + programs_locations[progindex]);
}

uint32_t vir2phys(void *address)
{
	uint32_t addr = (uint32_t) address;

	if (KERNEL_VIRTUAL_BASE <= addr && addr < (KERNEL_VIRTUAL_BASE + KERNEL_CODE_SIZE + KERNEL_DATA_SIZE))
	{
		return addr - KERNEL_VIRTUAL_BASE + KERNEL_PHYSICAL_BASE;
	}
	else if (KERNEL_RAW_ACCESS_BASE <= addr && addr < (KERNEL_RAW_ACCESS_BASE + OFFSET_16MB))
	{
		return addr - KERNEL_RAW_ACCESS_BASE;
	}
	else
	{
		panic("illegal address in vir2phys", NO_NUM);
	}       
}

void *phys2vir(uint32_t address)
{
	if (address < OFFSET_16MB)
	{
		return (void*)(address + KERNEL_RAW_ACCESS_BASE);
	}
	else
	{
		panic("illegal address in phys2vir", NO_NUM);
	}       
}

void copy_vir2phys(void *vir_src, uint32_t phys_dest, size_t len)
{
	void *vir_dest = phys2vir(phys_dest);
	kprintf("virt_dest = 0x%08X\n", vir_dest);
	memcpy(vir_dest, vir_src, len);

	// int *p = vir_dest;
	// kprintf("Contents of physical address 0:\n");
	// for (int i = 0; i < len/4; i++, p++)
	// {
	// 	kprintf("%2d: 0x%08X\n", i, *p);
	// }
}
