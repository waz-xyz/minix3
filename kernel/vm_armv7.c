#include "kernel.h"
#include "protect.h"
#include "proc.h"
#include <stdlib.h>
#include <string.h>
#include <elf.h>

#define	NOF_SECTIONS		(DDR_MEM_SIZE_IN_MB)
#define	NOF_PAGES_PER_SECTION	(ARM_SECTION_SIZE / SMALL_PAGE_SIZE)
#define	BITMAP_WSIZE		32
#define	BITMAP_BYTESIZE		(BITMAP_WSIZE / 8)
#define	BITMAP_LEN_PER_SECTION	(NOF_PAGES_PER_SECTION / BITMAP_WSIZE)
#define	BITMAP_SIZE_PER_SECTION	(NOF_PAGES_PER_SECTION / 8)

#define	IS_SECTION_EMPTY(entry)		(((entry) & 3) == 0)
#define	IS_PAGE_TABLE(entry)		(((entry) & 3) == 1)
#define	IS_PAGE_EMPTY(entry)		(((entry) & 3) == 0)
#define	GET_PAGE_TABLE_BASE(e)		((e) & ~PAGE_TABLE_ALIGN)
#define	IS_READABLE_SMALL_PAGE(e)	(((e) & 0x232U) == 0x22U)
#define	IS_WRITABLE_SMALL_PAGE(e)	(((e) & 0x232U) == 0x32U)

typedef struct
{
	int pages_in_use;
	short prev, next;
} section_entry;

static uint32_t *kernel_1st_level_tt = NULL;
static uint32_t *kernel_page_table = NULL;
static uint32_t next_free_user_tt_location = 0;

static section_entry sections_table[NOF_SECTIONS];
static uint32_t *pages_bitmap;

static void print_1st_level_table(uint32_t *table, int is_user);
static void print_page_table(uint32_t *table);
static void SetExceptionVectorTable(void);
static void SetPageTableDescriptor(uint32_t *tt, int index, uint32_t base, int domain, int ns);
static void SetSectionDescriptor(uint32_t *tt, int index, uint32_t base, int domain, int ap, int tex, int c, int b, int nG, int s);
static void SetSmallPageDescriptor(uint32_t *pt, int index, uint32_t base, int ap, int tex, int c, int b, int nG);
static void SetLongPageDescriptor(uint32_t *pt, int index, uint32_t base, int ap, int tex, int c, int b, int nG, int s);
static void MapSystemRegisters(void);
static void MarkPhysicalRangeAsUsedInSection(int section_inx, int first_page, int last_page);
static void MarkPhysicalRangeAsUsed(uint32_t start, uint32_t end);
static void InitPhysicalMemoryTracking(void);
static void *GetNextFreeUserTTLocation(uint32_t *phys_addr);
static uint32_t GetNextFreePageTableLocation(void);
static uint32_t GetNextFreeSmallPageLocation(void);
static uint32_t GetNextFreeLongPageLocation(void);
static uint32_t CountFreeMemory(void);

void init_mmu_module(void)
{
	InitPhysicalMemoryTracking();
	SetExceptionVectorTable();
	MapSystemRegisters();
	// kprintf("Kernel's first-level table:\n");
	// print_1st_level_table(kernel_1st_level_tt, 0);
	// kprintf("Kernel's page table:\n");
	// print_page_table(kernel_page_table);
}

static void SetExceptionVectorTable(void)
{
	uint32_t *pt = kernel_page_table + PAGE_TABLE_SIZE/4;
	uint32_t base = vir2phys(&exception_vector_start);
	SetSmallPageDescriptor(pt, 0xF0, base, AP_PL1_RO, 5, 0, 1, 0);
}

static void MapSystemRegisters(void)
{
	SetSectionDescriptor(kernel_1st_level_tt, 0xF80, 0xF8000000U, DOMAIN0, AP_PL1_RW, 2, 0, 0, 0, 0);
	SetSectionDescriptor(kernel_1st_level_tt, 0xF89, 0xF8900000U, DOMAIN0, AP_PL1_RW, 2, 0, 0, 0, 0);
	SetSectionDescriptor(kernel_1st_level_tt, 0xF8F, 0xF8F00000U, DOMAIN0, AP_PL1_RW, 2, 0, 0, 0, 0);
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

static void SetPageTableDescriptor(uint32_t *tt, int index, uint32_t base, int domain, int ns)
{
	tt[index] = (base & (~0x3FFU)) | (domain << 5) | (ns << 3) | 1;
}

static void SetSectionDescriptor(uint32_t *tt, int index, uint32_t base, int domain, int ap, int tex, int c, int b, int nG, int s)
{
	int ap2, ap10;
	ap2 = (ap >> 2) & 1;
	ap10 = ap & 3;
	tt[index] = (base & (~0xFFFFFU)) | (nG << 17) | (s << 16) | (ap2 << 15) | (tex << 12) | (ap10 << 10) |
		    (domain << 5) | (c << 3) | (b << 2) | 2;
}

static void SetSmallPageDescriptor(uint32_t *pt, int index, uint32_t base, int ap, int tex, int c, int b, int nG)
{
	int ap2, ap10;
	ap2 = (ap >> 2) & 1;
	ap10 = ap & 3;
	pt[index] = (base & (~0xFFF)) | (nG << 11) | (ap2 << 9) | (tex << 6) | (ap10 << 4) | (c << 3) | (b << 2) | 2;
}

static void SetLongPageDescriptor(uint32_t *pt, int index, uint32_t base, int ap, int tex, int c, int b, int nG, int s)
{
	int ap2, ap10, i;
	uint32_t entry;

	ap2 = (ap >> 2) & 1;
	ap10 = ap & 3;
	index &= ~(0xF);
	entry = (base & (~0xFFFFU)) | (tex << 12) | (nG << 11) | (s << 10) | (ap2 << 9) | (ap10 << 4) | (c << 3) | (b << 2) | 1;
	for (i = 0; i < 16; i++)
	{
		pt[index++] = entry;
	}
}

static void *GetNextFreeUserTTLocation(uint32_t *phys_addr)
{
	void *p;
	if (phys_addr != NULL)
	{
		*phys_addr = next_free_user_tt_location;
	}
	p = phys2vir(next_free_user_tt_location);
	next_free_user_tt_location += USER_FIRST_LEVEL_TT_SIZE;
	memset(p, 0, USER_FIRST_LEVEL_TT_SIZE);
	return p;
}

static uint32_t GetNextFreePageTableLocation(void)
{
	enum { PAGE_TABLES_IN_A_SMALL_PAGE = SMALL_PAGE_SIZE / PAGE_TABLE_SIZE };
	static uint32_t next_free_pt_location = 0;
	static int pt_cnt = 0;
	uint32_t pt_start;
	void *p;

	if (next_free_pt_location == 0 || pt_cnt >= PAGE_TABLES_IN_A_SMALL_PAGE)
	{
		next_free_pt_location = GetNextFreeSmallPageLocation();
		pt_cnt = 0;
	}
	pt_start = next_free_pt_location;
	pt_cnt++;
	next_free_pt_location += PAGE_TABLE_SIZE;
	memset(phys2vir(pt_start), 0, PAGE_TABLE_SIZE);
	//kprintf("New page table location: 0x%08X\n", pt_start);
	return pt_start;
}

static uint32_t GetNextFreeSmallPageLocation(void)
{
	section_entry *se;
	uint32_t base_addr;
	uint32_t bitmap;
	size_t base_inx;
	uint32_t bitmask;

	for (size_t i = 0; i < NOF_SECTIONS; i++)
	{
		se = &sections_table[i];
		if (se->pages_in_use < NOF_PAGES_PER_SECTION)
		{
			base_addr = i * ARM_SECTION_SIZE;
			base_inx = i * BITMAP_LEN_PER_SECTION;
			for (size_t j = 0; j < BITMAP_LEN_PER_SECTION; j++)
			{
				bitmap = pages_bitmap[base_inx + j];
				if (bitmap != ~0U)
				{
					for (int z = 0; z < BITMAP_WSIZE; z++)
					{
						bitmask = 1U << (BITMAP_WSIZE-1-z);
						if ((bitmap & bitmask) == 0)
						{
							pages_bitmap[base_inx + j] |= bitmask;
							return base_addr + (j * BITMAP_WSIZE + z) * SMALL_PAGE_SIZE;
						}
					}
				}
			}
		}
	}
	panic("Out of memory: not enough free pages", NO_NUM);
	return 0;
}

static uint32_t GetNextFreeLongPageLocation(void)
{
	section_entry *se;
	uint32_t base_addr;
	uint32_t bitmap;
	size_t base_inx;
	uint32_t bitmask;

	for (size_t i = 0; i < NOF_SECTIONS; i++)
	{
		se = &sections_table[i];
		if (se->pages_in_use < NOF_PAGES_PER_SECTION - SMALL_PAGES_IN_A_LONG_PAGE)
		{
			base_addr = i * ARM_SECTION_SIZE;
			base_inx = i * BITMAP_LEN_PER_SECTION;
			for (size_t j = 0; j < BITMAP_LEN_PER_SECTION; j++)
			{
				bitmap = pages_bitmap[base_inx + j];
				if ((bitmap & (0xFFFFU << 16)) == 0)
				{
					pages_bitmap[base_inx + j] |= (0xFFFFU << 16);
					return base_addr + (j * BITMAP_WSIZE) * SMALL_PAGE_SIZE;
				}
				else if ((bitmap & 0xFFFFU) == 0)
				{
					pages_bitmap[base_inx + j] |= 0xFFFFU;
					return base_addr + (j * BITMAP_WSIZE + 16) * SMALL_PAGE_SIZE;
				}
			}
		}
	}
	panic("Out of memory: not enough free long pages", NO_NUM);
	return 0;
}

void allocate_page_tables(struct proc *pr)
{
	uint32_t *tt;
	uint32_t virt_start, virt_end;
	struct mem_map *mm;
	
	tt = GetNextFreeUserTTLocation(&pr->p_ttbase);
	// kprintf("1st-level table for %s: 0x%08X\n", pr->p_name, pr->p_ttbase);
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
				SetPageTableDescriptor(tt, inx, GetNextFreePageTableLocation(), DOMAIN0, 0);
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
		ap = (seg == T) ? AP_PL0_RO : AP_PL0_RW;
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
			SetSmallPageDescriptor(pt, inx, base, ap, 5, 0, 1, 1);
		}
	}

	if (pr->p_nr == 0)
	{
		kprintf("MMU tables for %s:\n", pr->p_name);
		kprintf("p_ttbase = 0x%08X\n", pr->p_ttbase);
		print_mmu_tables(tt, 1);
	}

	if (pr->p_nr == 4)
	{
		kprintf("Kernel's first-level table:\n");
		print_1st_level_table(kernel_1st_level_tt, 0);
		kprintf("Kernel page table #1:\n");
		print_page_table(kernel_page_table);
		kprintf("Kernel page table #2:\n");
		print_page_table(kernel_page_table + PAGE_TABLE_SIZE/4);
	}
}

uint32_t allocate_task_stack(void)
{
	int i;

	/* Find the highest entry in the kernel's page table #0
	 * occupied by a stack.
	 */
	i = PAGE_TABLE_NOF_ENTRIES - 1;
	while ((kernel_page_table[i] & 3) != 0)
	{
		if ((kernel_page_table[i-1] & 3) == 0)
			i -= 2;
		else
			i -= 1;
	}

	if ((kernel_page_table[i-1] & 3) == 0)
	{
		SetSmallPageDescriptor(kernel_page_table, i, GetNextFreeSmallPageLocation(),
				       AP_PL1_RW, 5, 0, 1, 0);
		// New stack pointer sits at the next page boundary
		return KERNEL_VIRTUAL_BASE + (i+1) * SMALL_PAGE_SIZE;
	}
	else
	{
		panic("Not enough space to allocate a new task stack", NO_NUM);
	}
	
	return 0;
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
	else if (KERNEL_RAW_ACCESS_BASE <= addr && (addr - KERNEL_RAW_ACCESS_BASE) < MAX_PHYSICAL_MEMORY)
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
	if (address < MAX_PHYSICAL_MEMORY)
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
}

uint32_t get_asid(void)
{
	static uint32_t asid_cnt = 1;

	return asid_cnt++;
}

void release_asid(uint32_t asid)
{
	;
}

static void MarkPhysicalRangeAsUsedInSection(int section_inx, int first_page, int last_page)
{
	section_entry* se;
	size_t base_inx;
	uint32_t *pb;
	int i;
	uint32_t bitmask;
	
	se = &sections_table[section_inx];
	base_inx = section_inx*BITMAP_LEN_PER_SECTION;
	for (i = first_page; i < last_page; i++)
	{
		pb = &pages_bitmap[base_inx + i / BITMAP_WSIZE];
		bitmask = 1U << (BITMAP_WSIZE - 1 - i % BITMAP_WSIZE);
		if ((*pb & bitmask) == 0)
		{
			se->pages_in_use += 1;
			*pb |= bitmask;
		}
	}
}

static void MarkPhysicalRangeAsUsed(uint32_t start, uint32_t end)
{
	int firstSection, lastSection, lastBitmapWord;
	uint32_t firstPage, lastPage, bitmask;
	int i;
	section_entry* se;
	uint32_t *pb;
	size_t base_inx;

	if (start >= end)
	{
		panic("Bad physical address range", NO_NUM);
	}

	firstSection = (start >> ARM_SECTION_BITLEN);
	firstPage = (start & ARM_SECTION_ALIGN) >> SMALL_PAGE_BITLEN;
	lastSection = (end >> ARM_SECTION_BITLEN);
	lastPage = (end & ARM_SECTION_ALIGN) >> SMALL_PAGE_BITLEN;
	if ((end & SMALL_PAGE_ALIGN) != 0)
	{
		lastPage++;
	}
	//kprintf("start = 0x%08X, end = 0x%08X, firstSection = 0x%X, lastSection = 0x%X, firstPage = 0x%X, lastPage = 0x%X\n", start, end, firstSection, lastSection, firstPage, lastPage);

	if (firstSection == lastSection)
	{
		MarkPhysicalRangeAsUsedInSection(firstSection, firstPage, lastPage);
	}
	else
	{
		if (firstPage != 0)
		{
			MarkPhysicalRangeAsUsedInSection(firstSection, firstPage, NOF_PAGES_PER_SECTION);
			firstSection++;
		}
		for (i = firstSection; i < lastSection; i++)
		{
			se = &sections_table[i];
			se->pages_in_use = NOF_PAGES_PER_SECTION;
			memset(&pages_bitmap[i*BITMAP_LEN_PER_SECTION], ~0, BITMAP_SIZE_PER_SECTION);
		}
		if (lastPage != 0)
		{
			MarkPhysicalRangeAsUsedInSection(lastSection, 0, lastPage);
		}
	}
}

static uint32_t CountFreeMemory(void)
{
	int i;
	uint32_t freePages = 0;

	for (i = 0; i < NOF_SECTIONS; i++)
	{
		freePages += NOF_PAGES_PER_SECTION - sections_table[i].pages_in_use;
	}

	return freePages * SMALL_PAGE_SIZE;
}

static void ExpandDataSegment(void)
{
	int i, j;
	uint32_t base;

	// Find the highest occupied entry in the lower part of the kernel's page table #1
	for (i = 0; i < PAGE_TABLE_NOF_ENTRIES; i++)
	{
		if ((kernel_page_table[i] & 3) == 0)
			break;
	}
	base = GetNextFreeLongPageLocation();
	memset(phys2vir(base), 0, LARGE_PAGE_SIZE);
	SetLongPageDescriptor(kernel_page_table, i, base, AP_PL1_RW, 5, 0, 1, 0, 1);
}

static void InitPhysicalMemoryTracking(void)
{
	uint32_t kernel_stack_start, kernel_stack_end;
	uint32_t kernel_mmu_tables_start, kernel_mmu_tables_end;
	uint32_t user_tt_start, user_tt_end;
	uint32_t end_of_pages_bitmap, end_of_data_segment;
	uint32_t pages_bitmap_size;
	int i;
	extern char end, __data_start;
	int doExpansion = 0;

	kernel_stack_start = KERNEL_PHYSICAL_BASE + ALIGN_TO_SMALL_PAGE(end_of_image);
	/* Add space for kernel stack */
	kernel_stack_end = kernel_stack_start + KERNEL_STACK_SIZE;

	/* Align next position to a 1st-level table */
	kernel_mmu_tables_start = ALIGN_TO_POWER_OF_2(kernel_stack_end, KERNEL_FIRST_LEVEL_TT_SIZE);
	kernel_1st_level_tt = phys2vir(kernel_mmu_tables_start);
	kernel_page_table = phys2vir(kernel_mmu_tables_start + KERNEL_FIRST_LEVEL_TT_SIZE);
	/* Add space for kernel's MMU tables */
	kernel_mmu_tables_end = kernel_mmu_tables_start + KERNEL_MMU_TABLES_SIZE;

	/* Align to next possible user 1st-level table */
	user_tt_start = ALIGN_TO_POWER_OF_2(kernel_mmu_tables_end, USER_FIRST_LEVEL_TT_SIZE);
	next_free_user_tt_location = user_tt_start;
	user_tt_end = user_tt_start + (number_of_programs - 1) * USER_FIRST_LEVEL_TT_SIZE;

	/* Allocate pages_bitmap */
	pages_bitmap = (uint32_t*) ALIGN_TO_POWER_OF_2((uint32_t) (&end), 4);
	pages_bitmap_size = NOF_SECTIONS * BITMAP_SIZE_PER_SECTION;
	end_of_pages_bitmap = (uint32_t)(pages_bitmap) + pages_bitmap_size;
	end_of_data_segment = (uint32_t) (&__data_start) + LARGE_PAGE_SIZE;
	if (end_of_pages_bitmap > end_of_data_segment)
	{
		// We need to do this later to take into account lower portions
		// of memory that are already in use.
		kprintf("pages_bitmap = 0x%08X, pages_bitmap_size = %d\n", pages_bitmap, pages_bitmap_size);
		kprintf("end of pages_bitmap = 0x%08X\n", end_of_pages_bitmap);
		kprintf("current end of data segment = 0x%08X\n", end_of_data_segment);
		doExpansion = 1;
	}

	for (i = 0; i < NOF_SECTIONS; i++)
	{
		sections_table[i].pages_in_use = 0;
	}
	// The lower portion of pages_bitmap is already set to zero because is part of BSS

	/* First megabyte will be marked as used although is actually inaccessible */
	MarkPhysicalRangeAsUsed(0, kernel_stack_end);
	MarkPhysicalRangeAsUsed(kernel_mmu_tables_start, kernel_mmu_tables_end);
	MarkPhysicalRangeAsUsed(user_tt_start, user_tt_end);

	if (doExpansion)
	{
		ExpandDataSegment();
	}

	kinfo.free_mem = CountFreeMemory();

	// kprintf("user_tt_start: 0x%08X\n", user_tt_start);
	// kprintf("user_tt_end: 0x%08X\n", user_tt_end);
	// kprintf("kernel_mmu_tables_start: 0x%08X\n", kernel_mmu_tables_start);
	// kprintf("kernel_mmu_tables_end: 0x%08X\n", kernel_mmu_tables_end);
	// kprintf("kernel_page_table: 0x%08X\n", kernel_page_table);
}

void *validate_user_ptr(int proc_nr, void *ptr, size_t len, int type)
{
	/* len >= 1 */
	struct proc *pr = proc_addr(proc_nr);
	uint32_t *tt = phys2vir(pr->p_ttbase);
	uint32_t addr_start = (uint32_t) ptr;
	uint32_t addr_end = addr_start + len - 1;
	int sect_inx_start = addr_start >> ARM_SECTION_BITLEN;
	int page_inx_start = (addr_start & ARM_SECTION_ALIGN) >> SMALL_PAGE_BITLEN;
	int sect_inx_end = addr_end >> ARM_SECTION_BITLEN;
	int page_inx_end = (addr_end & ARM_SECTION_ALIGN) >> SMALL_PAGE_BITLEN;

	if (len == 0)
	{
		panic("validate_user_ptr: len = 0", NO_NUM);
	}
	if (sect_inx_start > MAX_USER_SECTION || sect_inx_end > MAX_USER_SECTION)
	{
		kprintf("Invalid user ptr: %p\nBad section\n", ptr);
		return NULL;
	}
	// kprintf("sect_inx_start = %d, sect_inx_end = %d\n", sect_inx_start, sect_inx_end);
	for (int i = sect_inx_start; i <= sect_inx_end; i++)
	{
		// kprintf("page table #0x%X: \n", i);
		if (!IS_PAGE_TABLE(tt[i]))
		{
			kprintf("Invalid user ptr: %p\nPage table not found\n", ptr);
			return NULL;
		}
		uint32_t *pt = phys2vir(GET_PAGE_TABLE_BASE(tt[i]));
		int start = (i == sect_inx_start) ? page_inx_start : 0;
		int end = (i == sect_inx_end) ? page_inx_end : (NOF_PAGES_PER_SECTION - 1);
		for (int j = start; j <= end; j++)
		{
			uint32_t entry = pt[j];
			// kprintf("page entry 0x%X: 0x%08X\n", j, entry);
			if (type == PTR_WRITABLE && !IS_WRITABLE_SMALL_PAGE(entry) ||
			    type == PTR_READABLE && !IS_READABLE_SMALL_PAGE(entry))
			{
				kprintf("Invalid user ptr: %p\nInvalid entry\n", ptr);
				return NULL;
			}
		}
	}
	
	return ptr;
}
