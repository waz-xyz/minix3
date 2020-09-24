#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdint.h>
#define	u64_t	uint64_t
#include "mkfs.h"

#define PATH_MAX           255	/* # chars in a path name */

typedef unsigned long block_t;

#ifndef __minix

#define BUF_SIZE 512		   /* size of the /etc/mtab buffer */
char* etc_mtab = "/etc/mtab";	   /* name of the /etc/mtab file */
static char mtab_in[BUF_SIZE + 1]; /* holds /etc/mtab when it is read in */
static char* iptr = mtab_in;	   /* pointer to next line to feed out. */

void std_err(char* s)
{
	register char* p = s;

	while (*p != 0)
		p++;
	(void)write(2, s, (int)(p - s));
}

void err(char* prog_name, char* str)
{
	std_err(prog_name);
	std_err(str);
	std_err(etc_mtab);
	perror(" ");
}

#endif

/* Read in /etc/mtab and store it in /etc/mtab. */
int load_mtab(char* prog_name)
{
	int fd, n;
	char* ptr;

	/* Open the file. */
	printf("load_mtab: opening %s\n", etc_mtab);
	fd = open(etc_mtab, O_RDONLY);
	if (fd < 0)
	{
		err(prog_name, ": cannot open ");
		return (-1);
	}

	/* File opened.  Read it in. */
	n = read(fd, mtab_in, BUF_SIZE);
	if (n <= 0)
	{
		/* Read failed. */
		err(prog_name, ": cannot read ");
		return (-1);
	}
	if (n == BUF_SIZE)
	{
		/* Some nut has mounted 50 file systems or something like that. */
		std_err(prog_name);
		std_err(": file too large: ");
		std_err(etc_mtab);
		return (-1);
	}

	close(fd);

	/* Replace all the whitespace by '\0'. */
	ptr = mtab_in;
	while (*ptr != '\0')
	{
		if (isspace(*ptr))
			*ptr = '\0';
		ptr++;
	}
	return (0);
}

int get_mtab_entry(char* special, char* mounted_on, char* version, char* rw_flag)
{
	/* Return the next entry from mtab_in. */

	if (iptr >= &mtab_in[BUF_SIZE])
	{
		special[0] = '\0';
		return (-1);
	}

	strcpy(special, iptr);
	while (isprint(*iptr))
		iptr++;
	while (*iptr == '\0' && iptr < &mtab_in[BUF_SIZE])
		iptr++;

	strcpy(mounted_on, iptr);
	while (isprint(*iptr))
		iptr++;
	while (*iptr == '\0' && iptr < &mtab_in[BUF_SIZE])
		iptr++;

	strcpy(version, iptr);
	while (isprint(*iptr))
		iptr++;
	while (*iptr == '\0' && iptr < &mtab_in[BUF_SIZE])
		iptr++;

	strcpy(rw_flag, iptr);
	while (isprint(*iptr))
		iptr++;
	while (*iptr == '\0' && iptr < &mtab_in[BUF_SIZE])
		iptr++;
	return (0);
}

/*================================================================
 *			other stuff
 *===============================================================*/

void pexit(char* s)
{
	fprintf(stderr, "%s: %s\n", progname, s);
	if (lct != 0)
		fprintf(stderr, "Line %d being processed when error detected.\n", lct);
	flush();
	exit(2);
}

void check_mtab(char* devname /* /dev/hd1 or whatever */)
{
	/* Check to see if the special file named in s is mounted. */

	int n, r;
	struct stat sb;
	char special[PATH_MAX + 1], mounted_on[PATH_MAX + 1], version[10], rw_flag[10];

	r = stat(devname, &sb);
	if (r == -1)
	{
		if (errno == ENOENT)
			return; /* Does not exist, and therefore not mounted. */
		fprintf(stderr, "%s: stat %s failed: %s\n", progname, devname, strerror(errno));
		exit(1);
	}
	return;
	//if (!S_ISBLK(sb.st_mode))
	//{
	//	/* Not a block device and therefore not mounted. */
	//	return;
	//}

	if (load_mtab("mkfs") < 0)
		return;
	while (1)
	{
		n = get_mtab_entry(special, mounted_on, version, rw_flag);
		if (n < 0)
			return;
		if (strcmp(devname, special) == 0)
		{
			/* Can't mkfs on top of a mounted file system. */
			fprintf(stderr, "%s: %s is mounted on %s\n",
				progname, devname, mounted_on);
			exit(1);
		}
	}
}

time_t file_time(int f)
{
	struct stat statbuf;
	fstat(f, &statbuf);
	return statbuf.st_mtime;
}

time_t filename_time(const char *fn)
{
	struct stat statbuf;
	stat(fn, &statbuf);
	return statbuf.st_mtime;
}

void *file_fopen(const char *name, const char *mode)
{
	return fopen(name, mode);
}

int file_fgetc(void* fp)
{
	return fgetc((FILE*)fp);
}

void special(char* string)
{
	fd = creat(string, 0777);
	if (fd < 0)
	{
		eprintf("Can't create special file %s\n", string);
		exit(3);
	}
	close(fd);
	fd = open(string, O_RDWR);
	if (fd < 0)
		pexit("Can't open special file");
}

/*================================================================
 *                    sizeup  -  determine device size
 *===============================================================*/
block_t sizeup(char* device)
{
	int fd;
	struct partition entry;
	block_t d;
	struct stat st;

	if ((fd = open(device, O_RDONLY)) == -1)
	{
		if (errno != ENOENT)
			perror("sizeup open");
		return 0;
	}
	if (fstat(fd, &st) < 0)
	{
		perror("fstat");
		entry.size = 0;
	}
	else
	{
		entry.size = st.st_size;
	}
	close(fd);
	d = (unsigned long) (entry.size / block_size);
	return d;
}

void file_sync(void)
{
	
}

long get_current_time(void)
{
	return (long) time(NULL);
}

const char* get_errno_string(void)
{
	return strerror(errno);
}

int eprintf(const char* fmt, ...)
{
	int ret;
	va_list args;

	va_start(args, fmt);
	ret = vfprintf(stderr, fmt, args);
	va_end(args);
	return ret;
}
