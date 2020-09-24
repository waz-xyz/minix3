#ifndef MKFS_H_

#define	MKFS_H_

#define	EOF	(-1)

/** minix/partition.h ************************************************************/

struct partition {
	u64_t base;		/* byte offset to the partition start */
	u64_t size;		/* number of bytes in the partition */
	unsigned cylinders;	/* disk geometry */
	unsigned heads;
	unsigned sectors;
};

/*********************************************************************************/

typedef unsigned long Ino_t;

extern char* progname;
extern unsigned int block_size;
extern int fd;
extern int lct;

void flush(void);
void file_sync(void);
void* file_fopen(const char* name, const char* mode);
int file_fgetc(void* fp);
long get_current_time(void);
const char* get_errno_string(void);
int eprintf(const char* fmt, ...);

#endif /* MKFS_H_ */
