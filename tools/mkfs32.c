/* mkfs  -  make the MINIX filesystem	Authors: Tanenbaum et al. */

/*	Authors: Andy Tanenbaum, Paul Ogilvie, Frans Meulenbroeks, Bruce Evans
 *
 * This program can make both version 1 and version 2 file systems, as follows:
 *	mkfs /dev/fd0 1200	# Version 2 (default)
 *	mkfs -1 /dev/fd0 360	# Version 1
 *
 */

#define _XOPEN_SOURCE 500

#ifdef __LP64__

#define	long	int

#endif

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <tools.h>
#include "../servers/fs/const.h"
#undef EXTERN
#define EXTERN /* get rid of EXTERN by making it null */
#include "../servers/fs/type.h"
#include "../servers/fs/super.h"
#include "mkfs.h"

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

/** limits.h ************************************************************/

#define LINK_MAX      SHRT_MAX	/* # links a file may have */
#define MAX_CANON          255	/* size of the canonical input queue */
#define MAX_INPUT          255	/* size of the type-ahead buffer */
#define NAME_MAX        DIRSIZ	/* # chars in a file name */
#define PATH_MAX           255	/* # chars in a path name */
#define PIPE_BUF          7168	/* # bytes in atomic write to a pipe */
#define STREAM_MAX          20	/* must be the same as FOPEN_MAX in stdio.h */
#define TZNAME_MAX           3	/* maximum bytes in a time zone name is 3 */
#define SSIZE_MAX        32767	/* max defined byte count for read() */

/*********************************************************************************/

/** minix/const.h ****************************************************************/

/* Flag bits for i_mode in the inode. */
#define I_TYPE          0170000	/* this field gives inode type */
#define I_SYMBOLIC_LINK 0120000	/* file is a symbolic link */
#define I_REGULAR       0100000	/* regular file, not dir or special */
#define I_BLOCK_SPECIAL 0060000	/* block special file */
#define I_DIRECTORY     0040000	/* file is a directory */
#define I_CHAR_SPECIAL  0020000	/* character special file */
#define I_NAMED_PIPE	0010000 /* named pipe (FIFO) */
#define I_SET_UID_BIT   0004000	/* set effective uid_t on exec */
#define I_SET_GID_BIT   0002000	/* set effective gid_t on exec */
#define ALL_MODES       0006777	/* all bits for user, group and others */
#define RWX_MODES       0000777	/* mode bits for RWX only */
#define R_BIT           0000004	/* Rwx protection bit */
#define W_BIT           0000002	/* rWx protection bit */
#define X_BIT           0000001	/* rwX protection bit */
#define I_NOT_ALLOC     0000000	/* this inode is free */

/* Some limits. */
#define MAX_BLOCK_NR  ((block_t) 077777777)	/* largest block number */
#define HIGHEST_ZONE   ((zone_t) 077777777)	/* largest zone number */
#define MAX_INODE_NR ((ino_t) 037777777777)	/* largest inode number */
#define MAX_FILE_POS ((off_t) 037777777777)	/* largest legal file offset */

#define MAX_SYM_LOOPS	8	/* how many symbolic links are recursed */

#define NO_BLOCK              ((block_t) 0)	/* absence of a block number */
#define NO_ENTRY                ((ino_t) 0)	/* absence of a dir entry */
#define NO_ZONE                ((zone_t) 0)	/* absence of a zone number */
#define NO_DEV                  ((dev_t) 0)	/* absence of a device numb */

/*********************************************************************************/

#define INODE_MAP 2
#define MAX_TOKENS 10
#define LINE_LEN 200
#define BIN 2
#define BINGRP 2
#define BIT_MAP_SHIFT 13
#define N_BLOCKS MAX_BLOCK_NR
#define N_BLOCKS16 (128L * 1024)
#define INODE_MAX ((unsigned)65535)

#define _MIN_BLOCK_SIZE 1024
#define _MAX_BLOCK_SIZE 4096
#define _STATIC_BLOCK_SIZE 1024

/* You can make a really large file system on a 16-bit system, but the array
 * of bits that get_block()/putblock() needs gets a bit big, so we can only
 * prefill MAX_INIT blocks.  (16-bit fsck can't check a file system larger
 * than N_BLOCKS16 anyway.)
 */
#define MAX_INIT	(N_BLOCKS)

extern char *optarg;
extern int optind;

int next_zone, next_inode, zone_size, zone_shift = 0, zoff;
block_t nrblocks;
int inode_offset, lct = 0, disk, fd, print = 0, file = 0;
unsigned int nrinodes;
int override = 0, simple = 0, dflag;
int donttest; /* skip test if it fits on medium */
char *progname;

time_t current_time, bin_time;
char *zero, *lastp;
char umap[MAX_INIT / 8]; /* bit map tells if block read yet */
block_t zone_map;	 /* where is zone map? (depends on # inodes) */
int inodes_per_block;
unsigned int block_size;
block_t max_nrblocks;

void *proto;

block_t sizeup(char *device);
void super(zone_t zones, Ino_t inodes);
void rootdir(Ino_t inode);
void eat_dir(Ino_t parent);
void eat_file(Ino_t inode, int f);
void enter_dir(Ino_t parent, char *name, Ino_t child);
void incr_size(Ino_t n, long count);
static ino_t alloc_inode(int mode, int usrid, int grpid);
static zone_t alloc_zone(void);
void add_zone(Ino_t n, zone_t z, long bytes, time_t cur_time);
void incr_link(Ino_t n);
void insert_bit(block_t block, int bit);
int mode_con(char *p);
void get_line(char line[LINE_LEN], char *parse[MAX_TOKENS]);
void check_mtab(char *devname);
time_t file_time(int f);
time_t filename_time(const char* fn);
void pexit(char *s);
void copy(const char *from, char *to, int count);
void print_fs(void);
int read_and_set(block_t n);
void special(char *string);
void get_block(block_t n, char *buf);
void get_super_block(char *buf);
void put_block(block_t n, char *buf);
void cache_init(void);
void flush(void);
void mx_read(int blocknr, char *buf);
void mx_write(int blocknr, char *buf);
void dexit(char *s, int sectnum, int err);
void usage(void);
char *alloc_block(void);

/*================================================================
 *                    mkfs  -  make filesystem
 *===============================================================*/
int main(int argc, char *argv[])
{
	int nread, mode, usrid, grpid, ch;
	block_t blocks, maxblocks;
	block_t i;
	ino_t root_inum;
	ino_t inodes;
	zone_t zones;
	char *token[MAX_TOKENS], line[LINE_LEN];

	// d2_inode ino;
	// printf("sizeof d2_mode = %zu\n", sizeof(ino.d2_mode));
	// printf("sizeof d2_nlinks = %zu\n", sizeof(ino.d2_nlinks));
	// printf("sizeof d2_uid = %zu\n", sizeof(ino.d2_uid));
	// printf("sizeof d2_gid = %zu\n", sizeof(ino.d2_gid));
	// printf("sizeof d2_size = %zu\n", sizeof(ino.d2_size));
	// printf("sizeof d2_atime = %zu\n", sizeof(ino.d2_atime));
	// printf("sizeof d2_mtime = %zu\n", sizeof(ino.d2_mtime));
	// printf("sizeof d2_ctime = %zu\n", sizeof(ino.d2_ctime));
	// printf("sizeof d2_zone[0] = %zu\n", sizeof(ino.d2_zone[0]));
	// printf("sizeof d2_zone = %zu\n", sizeof(ino.d2_zone));

	/* Get two times, the current time and the mod time of the binary of
	 * mkfs itself.  When the -d flag is used, the later time is put into
	 * the i_mtimes of all the files.  This feature is useful when
	 * producing a set of file systems, and one wants all the times to be
	 * identical. First you set the time of the mkfs binary to what you
	 * want, then go.
	 */
	current_time = get_current_time(); /* time mkfs is being run */
	bin_time = filename_time(argv[0]); /* time when mkfs binary was last modified */

	/* Process switches. */
	progname = argv[0];
	blocks = 0;
	i = 0;
	inodes_per_block = 0;
	max_nrblocks = N_BLOCKS;
	block_size = 0;
	while ((ch = getopt(argc, argv, "b:di:lotB:")) != -1)
		switch (ch)
		{
		case 'b':
			blocks = strtoul(optarg, (char **)NULL, 0);
			break;
		case 'd':
			dflag = 1;
			current_time = bin_time;
			break;
		case 'i':
			i = strtoul(optarg, (char **)NULL, 0);
			break;
		case 'l':
			print = 1;
			break;
		case 'o':
			override = 1;
			break;
		case 't':
			donttest = 1;
			break;
		case 'B':
			block_size = atoi(optarg);
			printf("block size = %d\n", block_size);
			break;
		default:
			usage();
		}

	if (optind >= argc)
	{
		usage();
		return 1;
	}

	if (block_size == 0) block_size = _MAX_BLOCK_SIZE; /* V3 default block size */
	if (block_size % SECTOR_SIZE != 0 || block_size < _MIN_BLOCK_SIZE)
	{
		eprintf("block size must be multiple of sector (%d) "
				"and at least %d bytes\n",
			SECTOR_SIZE, _MIN_BLOCK_SIZE);
		pexit("specified block size illegal");
	}
	printf("V2_INODE_SIZE = %u\n", V2_INODE_SIZE);
	if (block_size % V2_INODE_SIZE != 0)
	{
		eprintf("block size must be a multiple of inode size (%d bytes)\n",
			V2_INODE_SIZE);
		pexit("specified block size illegal");
	}

	printf("V2_INODES_PER_BLOCK = %u\n", V2_INODES_PER_BLOCK(block_size));
	if (inodes_per_block == 0)
		inodes_per_block = V2_INODES_PER_BLOCK(block_size);

	printf("BITS_PER_BLOCK = %u\n", FS_BITS_PER_BLOCK(block_size));

	/* now that the block size is known, do buffer allocations where
	 * possible.
	*/
	zero = alloc_block();
	//bzero(zero, block_size);

	/* Determine the size of the device if not specified as -b or proto. */
	maxblocks = sizeup(argv[optind]);
	if (argc - optind == 1 && blocks == 0)
	{
		pexit("Not supposed to be here");
		blocks = maxblocks;
		/* blocks == 0 is checked later, but leads to a funny way of
  		 * reporting a 0-sized device (displays usage).
  		 */
		if (blocks < 1)
		{
			eprintf("%s: this device can't hold a filesystem.\n",
				progname);
			return 1;
		}
	}

	/* The remaining args must be 'special proto', or just 'special' if the
	 * no. of blocks has already been specified.
	 */
	if (argc - optind != 2 && (argc - optind != 1 || blocks == 0))
		usage();

	if (blocks > maxblocks)
	{
		eprintf("%s: %s: number of blocks too large for device.\n",
			progname, argv[optind]);
		return 1;
	}

	/* Check special. */
	check_mtab(argv[optind]);

	/* Check and start processing proto. */
	optarg = argv[++optind];
	if (optind < argc && (proto = file_fopen(optarg, "r")) != NULL)
	{
		/* Prototype file is readable. */
		lct = 1;
		get_line(line, token); /* skip boot block info */

		/* Read the line with the block and inode counts. */
		get_line(line, token);
		blocks = atol(token[0]);
		if (blocks > max_nrblocks)
			pexit("Block count too large");
		if (sizeof(char *) == 2 && blocks > N_BLOCKS16)
		{
			eprintf("%s: warning: FS is larger than the %dM that fsck can check!\n",
					progname, (int)(N_BLOCKS16 / (1024L * 1024)));
		}
		inodes = atoi(token[1]);

		/* Process mode line for root directory. */
		get_line(line, token);
		mode = mode_con(token[0]);
		usrid = atoi(token[1]);
		grpid = atoi(token[2]);
	}
	else
	{
		lct = 0;
		if (optind < argc)
		{
			/* Maybe the prototype file is just a size.  Check. */
			blocks = strtoul(optarg, (char **)NULL, 0);
			if (blocks == 0)
				pexit("Can't open prototype file");
		}
		if (i == 0)
		{
			int kb;
			kb = blocks * (max(block_size, 1024) / 1024);
			/* The default for inodes is 2 blocks per kb, rounded up
		 * to fill an inode block.  Above 20M, the average files are
		 * sure to be larger because it is hard to fill up 20M with
		 * tiny files, so reduce the default number of inodes.  This
		 * default can always be overridden by using the '-i option.
		 */
			i = kb / 2;
			if (kb >= 20000)
				i = kb / 3;
			if (kb >= 40000)
				i = kb / 4;
			if (kb >= 60000)
				i = kb / 5;
			if (kb >= 80000)
				i = kb / 6;
			if (kb >= 100000)
				i = kb / 7;

			/* round up to fill inode block */
			i += inodes_per_block - 1;
			i = i / inodes_per_block * inodes_per_block;
		}
		if (blocks < 5)
			pexit("Block count too small");
		if (blocks > max_nrblocks)
			pexit("Block count too large");
		if (i < 1)
			pexit("Inode count too small");
		inodes = (ino_t)i;

		/* Make simple file system of the given size, using defaults. */
		mode = 040777;
		usrid = BIN;
		grpid = BINGRP;
		simple = 1;
	}

	if (ULONG_MAX / block_size <= blocks - 1)
	{
		eprintf("Warning: too big for filesystem to currently\n");
		eprintf("run on (max 4GB), truncating.\n");
		blocks = ULONG_MAX / block_size;
	}

	nrblocks = blocks;
	nrinodes = inodes;

	/* Open special. */
	special(argv[--optind]);

	if (!donttest)
	{
		short *testb;
		ssize_t w;

		testb = (short *)alloc_block();

		/* Try writing the last block of partition or diskette. */
		if (lseek(fd, (off_t)(blocks - 1) * block_size, SEEK_SET) < 0)
		{
			pexit("couldn't lseek to last block to test size (1)");
		}
		testb[0] = 0x3245;
		testb[1] = 0x11FF;
		testb[block_size - 1] = 0x1F2F;
		if ((w = write(fd, (char *)testb, block_size)) != block_size)
		{
			if (w < 0)
				perror("write");
			printf("%zd/%u\n", w, block_size);
			pexit("File system is too big for minor device (write)");
		}
		file_sync(); /* flush write, so if error next read fails */
		if (lseek(fd, (off_t)(blocks - 1) * block_size, SEEK_SET) < 0)
		{
			pexit("couldn't lseek to last block to test size (2)");
		}
		testb[0] = 0;
		testb[1] = 0;
		nread = read(fd, (char *)testb, block_size);
		if (nread != block_size || testb[0] != 0x3245 || testb[1] != 0x11FF ||
		    testb[block_size - 1] != 0x1F2F)
		{
			if (nread < 0)
				perror("read");
			pexit("File system is too big for minor device (read)");
		}
		if (lseek(fd, (off_t)(blocks - 1) * block_size, SEEK_SET) < 0)
		{
			perror("lseek");
			pexit("test: lseek failed");
		}
		testb[0] = 0;
		testb[1] = 0;
		if (write(fd, (char *)testb, block_size) != block_size)
			pexit("File system is too big for minor device (write2)");
		if (lseek(fd, 0L, SEEK_SET) < 0)
		{
			perror("lseek");
			pexit("test: lseek failed");
		}
		free(testb);
	}

	/* Make the file-system */

	cache_init();

	put_block((block_t)0, zero); /* Write a null boot block. */

	zone_shift = 0; /* for future use */
	zones = nrblocks >> zone_shift;

	super(zones, inodes);

	root_inum = alloc_inode(mode, usrid, grpid);
	rootdir(root_inum);
	if (simple == 0)
		eat_dir(root_inum);

	if (print)
		print_fs();
	flush();
	return 0;

	/* NOTREACHED */
} /* end main */

/* Convert from bit count to a block count. The usual expression
 *
 *	(nr_bits + (1 << BITMAPSHIFT) - 1) >> BITMAPSHIFT
 *
 * doesn't work because of overflow.
 *
 * Other overflow bugs, such as the expression for N_ILIST overflowing when
 * s_inodes is just over V*_INODES_PER_BLOCK less than the maximum+1, are not
 * fixed yet, because that number of inodes is silly.
 */
/* The above comment doesn't all apply now bit_t is long.  Overflow is now
 * unlikely, but negative bit counts are now possible (though unlikely)
 * and give silly results.
 */
int bitmapsize(bit_t nr_bits, int block_size)
{
	int nr_blocks;

	nr_blocks = (int)(nr_bits / FS_BITS_PER_BLOCK(block_size));
	if (((bit_t)nr_blocks * FS_BITS_PER_BLOCK(block_size)) < nr_bits)
		++nr_blocks;
	return (nr_blocks);
}

/*================================================================
 *                 super  -  construct a superblock
 *===============================================================*/
void super(zone_t zones, ino_t inodes)
{
	int inodeblks;
	int initblks;

	zone_t initzones, nrzones, v2sq;
	zone_t zo;
	struct super_block *sup;
	char *buf;

	buf = alloc_block();
	sup = (struct super_block *)buf; /* lint - might use a union */

	sup->s_ninodes = inodes;
	sup->s_nzones = 0; /* not used in V2 - 0 forces errors early */
	sup->s_zones = zones;
	sup->s_imap_blocks = bitmapsize((bit_t)(1 + inodes), block_size);
	sup->s_zmap_blocks = bitmapsize((bit_t)zones, block_size);
	printf("s_imap_blocks = %d, s_zmap_blocks = %d\n", sup->s_imap_blocks, sup->s_zmap_blocks);
	inode_offset = sup->s_imap_blocks + sup->s_zmap_blocks + 2;
	inodeblks = (inodes + inodes_per_block - 1) / inodes_per_block;
	initblks = inode_offset + inodeblks;
	printf("inodeblks = %d, initblks = %d\n", inodeblks, initblks);
	initzones = (initblks + (1 << zone_shift) - 1) >> zone_shift;
	nrzones = nrblocks >> zone_shift;
	sup->s_firstdatazone = (initblks + (1 << zone_shift) - 1) >> zone_shift;
	printf("s_firstdatazone = %d\n", sup->s_firstdatazone);
	zoff = sup->s_firstdatazone - 1;
	sup->s_log_zone_size = zone_shift;

	v2sq = (zone_t)V2_INDIRECTS(block_size) * V2_INDIRECTS(block_size);
	zo = V2_NR_DZONES + (zone_t)V2_INDIRECTS(block_size) + v2sq;
	sup->s_magic = SUPER_V3;
	sup->s_block_size = block_size;
	sup->s_disk_version = 0;
	if (ULONG_MAX / block_size < zo)
	{
		sup->s_max_size = ULONG_MAX;
	}
	else
	{
		sup->s_max_size = zo * block_size;
	}

	zone_size = 1 << zone_shift; /* nr of blocks per zone */

	if (lseek(fd, (off_t)_STATIC_BLOCK_SIZE, SEEK_SET) < 0)
	{
		pexit("super() couldn't lseek");
	}
	if (write(fd, buf, _STATIC_BLOCK_SIZE) != _STATIC_BLOCK_SIZE)
	{
		pexit("super() couldn't write");
	}

	/* Clear maps and inodes. */
	for (int i = 2; i < initblks; i++)
		put_block((block_t)i, zero);

	next_zone = sup->s_firstdatazone;
	next_inode = 1;

	zone_map = INODE_MAP + sup->s_imap_blocks;

	insert_bit(zone_map, 0);	   /* bit zero must always be allocated */
	insert_bit((block_t)INODE_MAP, 0); /* inode zero not used but
					 * must be allocated */

	free(buf);
}

/*================================================================
 *              rootdir  -  install the root directory
 *===============================================================*/
void rootdir(ino_t inode)
{
	zone_t z;

	z = alloc_zone();
	add_zone(inode, z, 2 * sizeof(struct direct), current_time);
	enter_dir(inode, ".", inode);
	enter_dir(inode, "..", inode);
	incr_link(inode);
	incr_link(inode);
}

/*================================================================
 *	    eat_dir  -  recursively install directory
 *===============================================================*/
void eat_dir(ino_t parent)
{
	/* Read prototype lines and set up directory. Recurse if need be. */
	char *token[MAX_TOKENS], *p;
	char line[LINE_LEN];
	int mode, usrid, grpid, maj, min, f;
	ino_t n;
	zone_t z;
	long size;

	while (1)
	{
		get_line(line, token);
		p = token[0];
		if (*p == '$')
			return;
		p = token[1];
		mode = mode_con(p);
		usrid = atoi(token[2]);
		grpid = atoi(token[3]);
		if (grpid & 0200)
			eprintf("A.S.Tanenbaum\n");
		n = alloc_inode(mode, usrid, grpid);

		/* Enter name in directory and update directory's size. */
		enter_dir(parent, token[0], n);
		incr_size(parent, sizeof(struct direct));

		/* Check to see if file is directory or special. */
		incr_link(n);
		if (*p == 'd')
		{
			/* This is a directory. */
			z = alloc_zone(); /* zone for new directory */
			add_zone(n, z, 2 * sizeof(struct direct), current_time);
			enter_dir(n, ".", n);
			enter_dir(n, "..", parent);
			incr_link(parent);
			incr_link(n);
			eat_dir(n);
		}
		else if (*p == 'b' || *p == 'c')
		{
			/* Special file. */
			maj = atoi(token[4]);
			min = atoi(token[5]);
			size = 0;
			if (token[6])
				size = atoi(token[6]);
			size = block_size * size;
			add_zone(n, (zone_t)((maj << 8) | min), size, current_time);
		}
		else
		{
			/* Regular file. Go read it. */
			if ((f = open(token[4], O_RDONLY)) < 0)
			{
				eprintf("%s: Can't open %s: %s\n",
					progname, token[4], get_errno_string());
			}
			else
			{
				eat_file(n, f);
			}
		}
	}
}

/*================================================================
 * 		eat_file  -  copy file to MINIX
 *===============================================================*/
/* Zonesize >= blocksize */
void eat_file(ino_t inode, int f)
{
	int ct, i, j;
	zone_t z;
	char *buf;
	long timeval;

	buf = alloc_block();

	do
	{
		for (i = 0, j = 0; i < zone_size; i++, j += ct)
		{
			ct = read(f, buf, block_size);
			if (ct > 0)
			{
				if (i == 0)
					z = alloc_zone();
				put_block((z << zone_shift) + i, buf);
			}
		}
		timeval = (dflag ? current_time : file_time(f));
		if (ct != 0)
			add_zone(inode, z, (long)j, timeval);
	} while (ct == block_size);
	close(f);
	free(buf);
}

/*================================================================
 *	    directory & inode management assist group
 *===============================================================*/
void enter_dir(ino_t parent, char *name, ino_t child)
{
	/* Enter child in parent directory */
	/* Works for dir > 1 block and zone > block */
	int k, l, off;
	block_t b;
	zone_t z;
	char *p1, *p2;
	struct direct *dir_entry;
	d2_inode *ino2;
	int nr_dzones;

	b = ((parent - 1) / inodes_per_block) + inode_offset;
	off = (parent - 1) % inodes_per_block;

	if ((dir_entry = malloc(NR_DIR_ENTRIES(block_size) * sizeof(*dir_entry))) == NULL)
		pexit("couldn't allocate directory entry");

	if ((ino2 = malloc(V2_INODES_PER_BLOCK(block_size) * sizeof(*ino2))) == NULL)
		pexit("couldn't allocate block of inodes entry");

	get_block(b, (char *)ino2);
	nr_dzones = V2_NR_DZONES;

	for (k = 0; k < nr_dzones; k++)
	{
		z = ino2[off].d2_zone[k];
		if (z == 0)
		{
			z = alloc_zone();
			ino2[off].d2_zone[k] = z;
		}
		for (l = 0; l < zone_size; l++)
		{
			get_block((z << zone_shift) + l, (char *)dir_entry);
			for (size_t i = 0; i < NR_DIR_ENTRIES(block_size); i++)
			{
				if (dir_entry[i].d_ino == 0)
				{
					dir_entry[i].d_ino = child;
					p1 = name;
					p2 = dir_entry[i].d_name;
					size_t j = sizeof(dir_entry[i].d_name);
					while (j-- != 0)
					{
						*p2++ = *p1;
						if (*p1 != '\0')
							p1++;
					}
					put_block((z << zone_shift) + l, (char *)dir_entry);
					put_block(b, (char *)ino2);
					free(dir_entry);
					free(ino2);
					return;
				}
			}
		}
	}

	printf("Directory-inode %lu beyond direct blocks. Could not enter %s\n",
	       parent, name);
	pexit("Halt");
}

void add_zone(ino_t n, zone_t z, long bytes, time_t cur_time)
{
	/* Add zone z to inode n. The file has grown by 'bytes' bytes. */

	int off;
	block_t b;
	zone_t indir;
	zone_t *blk;
	d2_inode *p;
	d2_inode *inode;

	if (!(blk = malloc(V2_INDIRECTS(block_size) * sizeof(*blk))))
		pexit("Couldn't allocate indirect block");

	if (!(inode = malloc(V2_INODES_PER_BLOCK(block_size) * sizeof(*inode))))
		pexit("Couldn't allocate block of inodes");

	b = ((n - 1) / V2_INODES_PER_BLOCK(block_size)) + inode_offset;
	off = (n - 1) % V2_INODES_PER_BLOCK(block_size);
	get_block(b, (char *)inode);
	p = &inode[off];
	p->d2_size += bytes;
	p->d2_mtime = cur_time;
	for (int i = 0; i < V2_NR_DZONES; i++)
	{
		if (p->d2_zone[i] == 0)
		{
			p->d2_zone[i] = z;
			put_block(b, (char*)inode);
			free(blk);
			free(inode);
			return;
		}
	}
	put_block(b, (char *)inode);

	/* File has grown beyond a small file. */
	if (p->d2_zone[V2_NR_DZONES] == 0)
		p->d2_zone[V2_NR_DZONES] = alloc_zone();
	indir = p->d2_zone[V2_NR_DZONES];
	put_block(b, (char *)inode);
	b = indir << zone_shift;
	get_block(b, (char *)blk);
	for (size_t i = 0; i < V2_INDIRECTS(block_size); i++)
	{
		if (blk[i] == 0)
		{
			blk[i] = z;
			put_block(b, (char*)blk);
			free(blk);
			free(inode);
			return;
		}
	}
	pexit("File has grown beyond single indirect");
}

void incr_link(ino_t n)
{
	/* Increment the link count to inode n */
	int off;
	static int enter = 0;
	block_t b;
	size_t sz;
	static d2_inode* inode2 = NULL;

	if (enter)
		exit(1);

	b = ((n - 1) / inodes_per_block) + inode_offset;
	off = (n - 1) % inodes_per_block;
	
	sz = sizeof(*inode2) * V2_INODES_PER_BLOCK(block_size);
	if (!inode2 && !(inode2 = malloc(sz)))
		pexit("couldn't allocate a block of inodes");

	get_block(b, (char *)inode2);
	inode2[off].d2_nlinks++;
	put_block(b, (char *)inode2);
	enter = 0;
}

void incr_size(ino_t n, long count)
{
	/* Increment the file-size in inode n */
	block_t b;
	int off;

	b = ((n - 1) / inodes_per_block) + inode_offset;
	off = (n - 1) % inodes_per_block;
	d2_inode *inode2;
	if (!(inode2 = malloc(V2_INODES_PER_BLOCK(block_size) * sizeof(*inode2))))
		pexit("couldn't allocate a block of inodes");

	get_block(b, (char *)inode2);
	inode2[off].d2_size += count;
	put_block(b, (char *)inode2);
	free(inode2);
}

/*================================================================
 * 	 	     allocation assist group
 *===============================================================*/
static ino_t alloc_inode(int mode, int usrid, int grpid)
{
	ino_t num;
	int off;
	block_t b;
	d2_inode* inode2;

	num = next_inode++;
	if (num > nrinodes)
	{
		eprintf("have %d inodes\n", nrinodes);
		pexit("File system does not have enough inodes");
	}
	b = ((num - 1) / inodes_per_block) + inode_offset;
	off = (num - 1) % inodes_per_block;

	if (!(inode2 = malloc(V2_INODES_PER_BLOCK(block_size) * sizeof(*inode2))))
		pexit("couldn't allocate a block of inodes");

	get_block(b, (char *)inode2);
	inode2[off].d2_mode = mode;
	inode2[off].d2_uid = usrid;
	inode2[off].d2_gid = grpid;
	put_block(b, (char *)inode2);

	free(inode2);

	/* Set the bit in the bit map. */
	/* DEBUG FIXME.  This assumes the bit is in the first inode map block. */
	int bit_off = num % FS_BITS_PER_BLOCK(block_size);
	block_t inode_off = num / FS_BITS_PER_BLOCK(block_size);
	insert_bit(INODE_MAP + inode_off, bit_off);

	return num;
}

static zone_t alloc_zone(void)
{
	/* Allocate a new zone */
	/* Works for zone > block */
	block_t b;
	int i;
	zone_t z;

	z = next_zone++;
	b = z << zone_shift;
	if ((b + zone_size) > nrblocks)
		pexit("File system not big enough for all the files");
	for (i = 0; i < zone_size; i++)
		put_block(b + i, zero); /* give an empty zone */
	/* DEBUG FIXME.  This assumes the bit is in the first zone map block. */
	int bit = z - zoff;
	block_t zone_off = bit / FS_BITS_PER_BLOCK(block_size);
	int bit_off = bit % FS_BITS_PER_BLOCK(block_size);
	insert_bit(zone_map + zone_off, bit_off); /* lint, NOT OK because
						 * z hasn't been broken
						 * up into block +
						 * offset yet. */
	return z;
}

void insert_bit(block_t block, int bit)
{
	/* Insert 'count' bits in the bitmap */
	unsigned w, s;
	short *buf;

	buf = (short *)alloc_block();

	get_block(block, (char *)buf);
	w = bit / (8 * sizeof(short));
	s = bit % (8 * sizeof(short));
	if (w >= block_size/2)
	{
		eprintf("insert_bit: bit offset %d is out of bounds of block %lu", bit, block);
		exit(3);
	}
	buf[w] |= (1 << s);
	put_block(block, (char *)buf);

	free(buf);
}

/*================================================================
 * 		proto-file processing assist group
 *===============================================================*/
int mode_con(char *p)
{
	/* Convert string to mode */
	int o1, o2, o3, mode;
	char c1, c2, c3;

	c1 = *p++;
	c2 = *p++;
	c3 = *p++;
	o1 = *p++ - '0';
	o2 = *p++ - '0';
	o3 = *p++ - '0';
	mode = (o1 << 6) | (o2 << 3) | o3;
	if (c1 == 'd')
		mode += I_DIRECTORY;
	if (c1 == 'b')
		mode += I_BLOCK_SPECIAL;
	if (c1 == 'c')
		mode += I_CHAR_SPECIAL;
	if (c1 == '-')
		mode += I_REGULAR;
	if (c2 == 'u')
		mode += I_SET_UID_BIT;
	if (c3 == 'g')
		mode += I_SET_GID_BIT;
	return (mode);
}

void get_line(char line[LINE_LEN], char* parse[MAX_TOKENS])
{
	/* Read a line and break it up in tokens */
	int k;
	char c, *p;
	int d;

	for (k = 0; k < MAX_TOKENS; k++)
		parse[k] = 0;
	for (k = 0; k < LINE_LEN; k++)
		line[k] = 0;
	k = 0;
	parse[0] = 0;
	p = line;
	while (1)
	{
		if (++k > LINE_LEN)
			pexit("Line too long");
		d = file_fgetc(proto);
		if (d == EOF)
			pexit("Unexpected end-of-file");
		*p = d;
		if (*p == '\n')
			lct++;
		if (*p == ' ' || *p == '\t')
			*p = 0;
		if (*p == '\n')
		{
			*p++ = 0;
			*p = '\n';
			break;
		}
		p++;
	}

	k = 0;
	p = line;
	lastp = line;
	while (1)
	{
		c = *p++;
		if (c == '\n')
			return;
		if (c == 0)
			continue;
		parse[k++] = p - 1;
		do
		{
			c = *p++;
		} while (c != 0 && c != '\n');
	}
}

/*================================================================
 *			other stuff
 *===============================================================*/

void copy(const char *from, char *to, int count)
{
	while (count--)
		*to++ = *from++;
}

char *alloc_block(void)
{
	char *buf;

	if ((buf = calloc(1, block_size)) == NULL)
	{
		pexit("couldn't allocate filesystem buffer");
	}

	return buf;
}

void print_fs(void)
{
	int i;
	ino_t k;
	d2_inode *inode2;
	unsigned short *usbuf;
	block_t b;
	struct direct *dir;

	if ((inode2 = malloc(V2_INODES_PER_BLOCK(block_size) * sizeof(*inode2))) == NULL)
		pexit("couldn't allocate a block of inodes");

	if ((dir = malloc(NR_DIR_ENTRIES(block_size) * sizeof(*dir))) == NULL)
		pexit("malloc of directory entry failed");

	usbuf = (unsigned short *)alloc_block();

	get_super_block((char *)usbuf);
	printf("\nSuperblock: ");
	for (i = 0; i < 8; i++)
		printf("%06o ", usbuf[i]);
	get_block((block_t)2, (char *)usbuf);
	printf("...\nInode map:  ");
	for (i = 0; i < 9; i++)
		printf("%06o ", usbuf[i]);
	get_block((block_t)3, (char *)usbuf);
	printf("...\nZone  map:  ");
	for (i = 0; i < 9; i++)
		printf("%06o ", usbuf[i]);
	printf("...\n");

	free(usbuf);
	usbuf = NULL;

	k = 0;
	for (b = inode_offset; k < nrinodes; b++)
	{
		get_block(b, (char *)inode2);
		for (i = 0; i < inodes_per_block; i++)
		{
			k = inodes_per_block * (int)(b - inode_offset) + i + 1;
			/* Lint but OK */
			if (k > nrinodes)
				break;
			if (inode2[i].d2_mode != 0)
			{
				printf("Inode %2lu:  mode=", k);
				printf("%06o", inode2[i].d2_mode);
				printf("  uid=%2d  gid=%2d  size=",
					    inode2[i].d2_uid, inode2[i].d2_gid);
				printf("%6ld", inode2[i].d2_size);
				printf("  zone[0]=%ld\n", inode2[i].d2_zone[0]);
			}
			if ((inode2[i].d2_mode & I_TYPE) == I_DIRECTORY)
			{
				/* This is a directory */
				get_block(inode2[i].d2_zone[0], (char *)dir);
				for (size_t j = 0; j < NR_DIR_ENTRIES(block_size); j++)
					if (dir[j].d_ino)
						printf("\tInode %2lu: %s\n", dir[j].d_ino, dir[j].d_name);
			}
		}
	}

	printf("%d inodes used.     %d zones used.\n", next_inode - 1, next_zone);
	free(dir);
	free(inode2);
}

int read_and_set(block_t n)
{
/* The first time a block is read, it returns all 0s, unless there has
 * been a write. This routine checks to see if a block has been accessed.
 */
	int w, s, mask, r;

	w = n / 8;
	s = n % 8;
	mask = 1 << s;
	r = (umap[w] & mask) != 0;
	umap[w] |= mask;
	return r;
}

void usage(void)
{
	eprintf("Usage: %s [-12dlot] [-b blocks] [-i inodes] [-B blocksize] special [proto]\n",
			progname);
	exit(1);
}

/*================================================================
 *		      get_block & put_block
 *===============================================================*/

void get_block(block_t n, char *buf)
{
	/* Read a block. */

	int k;

	/* First access returns a zero block */
	if (read_and_set(n) == 0)
	{
		copy(zero, buf, block_size);
		return;
	}
	if (lseek(fd, (off_t)n * block_size, SEEK_SET) < 0)
	{
		pexit("get_block: lseek failed");
	}
	k = read(fd, buf, block_size);
	if (k != block_size)
	{
		eprintf("k = %d, block size = %d\n", k, block_size);
		pexit("get_block couldn't read");
	}
}

void get_super_block(char *buf)
{
	/* Read a block. */

	int k;

	if (lseek(fd, (off_t)SUPER_BLOCK_BYTES, SEEK_SET) < 0)
	{
		perror("lseek");
		pexit("lseek failed");
	}
	k = read(fd, buf, _STATIC_BLOCK_SIZE);
	if (k != _STATIC_BLOCK_SIZE)
	{
		pexit("get_super_block couldn't read");
	}
}

void put_block(block_t n, char *buf)
{
	/* Write a block. */

	(void)read_and_set(n);

	/* XXX - check other lseeks too. */
	if (lseek(fd, n * block_size, SEEK_SET) < 0)
	{
		pexit("put_block couldn't lseek");
	}
	if (write(fd, buf, block_size) != block_size)
	{
		pexit("put_block couldn't write");
	}
}

/* Dummy routines to keep source file clean from #ifdefs */

void flush(void)
{
	return;
}

void cache_init(void)
{
	return;
}
