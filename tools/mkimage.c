/*	mkimage 1.0 - Make a boot image                 Author: Kees J. Bot
 *								21 Dec 1991
 *
 * Make an image from kernel, mm, fs, etc.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <elf.h>

#define IM_NAME_MAX	63

struct image_header {
	char		name[IM_NAME_MAX + 1];	/* Null terminated. */
	Elf32_Ehdr	process;
};

#define PAGE_SIZE	4096	/* MMU's small page size */
#define LARGE_PAGE_SIZE	65536	/* MMU's large page size */

#define FLAGS_CODE	(PF_R | PF_X)
#define FLAGS_DATA	(PF_R | PF_W)

void report(char *label)
/* mkimage: label: No such file or directory */
{
	fprintf(stderr, "mkimage: %s: %s\n", label, strerror(errno));
}

void fatal(char *label)
{
	report(label);
	exit(1);
}

char *basename(char *name)
/* Return the last component of name, stripping trailing slashes from name.
 * Precondition: name != "/".  If name is prefixed by a label, then the
 * label is copied to the basename too.
 */
{
	static char base[IM_NAME_MAX];
	char *p, *bp= base;

	if ((p= strchr(name, ':')) != NULL) {
		while (name <= p && bp < base + IM_NAME_MAX - 1)
			*bp++ = *name++;
	}
	for (;;) {
		if ((p= strrchr(name, '/')) == NULL) { p= name; break; }
		if (*++p != 0) break;
		*--p= 0;
	}
	while (*p != 0 && bp < base + IM_NAME_MAX - 1) *bp++ = *p++;
	*bp= 0;
	return base;
}

void bread(FILE *f, char *name, void *buf, size_t len)
/* Read len bytes.  Don't dare return without them. */
{
	if (len > 0 && fread(buf, len, 1, f) != 1) {
		if (ferror(f)) fatal(name);
		fprintf(stderr, "mkimage: Unexpected EOF on %s\n", name);
		exit(1);
	}
}

void bwrite(FILE *f, char *name, void *buf, size_t len)
{
	if (len > 0 && fwrite(buf, len, 1, f) != 1) fatal(name);
}

long total_text = 0, total_data = 0, total_bss = 0;

void read_header(char *proc, FILE *procf, struct image_header *ihdr)
/* Read the ELF header of a program and check it.  If procf happens to be
 * NULL then the header is already in *image_hdr and need only be checked.
 */
{
	unsigned n;
	Elf32_Ehdr *ehdr = &(ihdr->process);

	if (procf == NULL) {
		/* Header already present. */
		n = ehdr->e_ehsize;
	} else {
		memset(ihdr, 0, sizeof(*ihdr));

		/* Put the basename of proc in the header. */
		strncpy(ihdr->name, basename(proc), IM_NAME_MAX);

		/* Read the header. */
		n = fread(ehdr, sizeof(char), sizeof(Elf32_Ehdr), procf);
		if (ferror(procf)) fatal(proc);
	}

	if (n < sizeof(Elf32_Ehdr)
		|| memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0
		|| ehdr->e_ident[4] != ELFCLASS32
		|| ehdr->e_type != ET_EXEC
		|| ehdr->e_phoff == 0
		|| ehdr->e_phnum == 0) {
		fprintf(stderr, "mkimage: %s is not a valid executable\n", proc);
		exit(1);
	}
}

void padimage(char *image, FILE *imagef, int n)
/* Add n zeros to image to pad it to a page boundary. */
{
	while (n > 0) {
		if (putc(0, imagef) == EOF) fatal(image);
		n--;
	}
}

#define align(n)	(((n) + ((PAGE_SIZE) - 1)) & ~((PAGE_SIZE) - 1))
#define large_align(n)	(((n) + ((LARGE_PAGE_SIZE) - 1)) & ~((LARGE_PAGE_SIZE) - 1))

void copysegment(char *proc, FILE *procf, char *image, FILE *imagef, long n, bool has_large_pages)
/* Copy n bytes from proc to image padded to fill a page. */
{
	int pad, c;

	/* Compute number of padding bytes. */
	pad = (has_large_pages ? large_align(n) : align(n)) - n;

	while (n > 0) {
		if ((c = getc(procf)) == EOF) {
			if (ferror(procf)) fatal(proc);
			fprintf(stderr,	"mkimage: premature EOF on %s\n", proc);
			exit(1);
		}
		if (putc(c, imagef) == EOF) fatal(image);
		n--;
	}
	padimage(image, imagef, pad);
}

void make_image(char *image, char **procv, int num_programs)
/* Collect a set of files in an image, each "segment" is nicely padded out
 * to PAGE_SIZE, so it may be read from disk into memory without trickery.
 */
{
	FILE *imagef, *procf;
	char *proc, *file;
	int procn;
	struct image_header ihdr;
	Elf32_Ehdr ehdr;
	Elf32_Phdr *phdr;
	struct stat st;
	int banner = 0;
	Elf32_Word text_size, data_size, bss_size;
	uint32_t *loc_table;

	if ((imagef = fopen(image, "w")) == NULL) fatal(image);

	loc_table = calloc(num_programs, sizeof(uint32_t));
	if (loc_table == NULL) {
		fatal(image);
	}

	for (procn = 0; (proc = *procv++) != NULL; procn++) {
		/* Remove the label from the file name. */
		if ((file = strchr(proc, ':')) != NULL) file++; else file = proc;

		/* Real files please, may need to seek. */
		if (stat(file, &st) < 0
			|| (errno = EISDIR, !S_ISREG(st.st_mode))
			|| (procf = fopen(file, "r")) == NULL
		) fatal(proc);

		/* Read ELF header. */
		read_header(proc, procf, &ihdr);

		/* Scratch. */
		ehdr = ihdr.process;

		if (ehdr.e_phentsize != sizeof(Elf32_Phdr)) {
			fprintf(stderr,
				"mkimage: %s: unknown segment header size (%u)\n",
				proc,
				ehdr.e_phentsize);
			exit(1);
		}

		/* Read segment headers into the phdr table */
		phdr = malloc(sizeof(Elf32_Phdr) * ehdr.e_phnum);
		if (phdr == NULL) {
			fatal(proc);
		}
		fseek(procf, ehdr.e_phoff, SEEK_SET);
		size_t nseg = fread(phdr, sizeof(Elf32_Phdr), ehdr.e_phnum, procf);
		if (ferror(procf)) fatal(proc);
		if (nseg != ehdr.e_phnum) {
			fprintf(stderr,
				"mkimage: %s: expected %u segment headers but got only %zu\n",
				proc,
				ehdr.e_phnum,
				nseg);
			exit(1);
		}

		/* Look through segments */
		text_size = data_size = bss_size = 0;
		for (int i = 0; i < ehdr.e_phnum; i++) {
			Elf32_Word segsize = phdr[i].p_filesz;
			if (phdr[i].p_type != PT_LOAD) {
				continue;
			}
			if (segsize == 0) {
				fseek(imagef, phdr[i].p_memsz, SEEK_CUR);
				continue;
			}

			Elf32_Word flags = phdr[i].p_flags;
			if ((flags & FLAGS_CODE) == FLAGS_CODE) {
				text_size = phdr[i].p_memsz;
				if (procn < num_programs) {
					loc_table[procn] = ftell(imagef);
				}
			} else if ((flags & FLAGS_DATA) == FLAGS_DATA) {
				data_size = phdr[i].p_filesz;
				bss_size = phdr[i].p_memsz - phdr[i].p_filesz;
			}

			fseek(procf, phdr[i].p_offset, SEEK_SET);
			copysegment(proc, procf, image, imagef, segsize, i < 2);
		}

		if (!banner) {
			printf("     text     data      bss      size\n");
			banner = 1;
		}
		printf(" %8u %8u %8u %9u  %s\n",
		       text_size, data_size, bss_size,
		       text_size + data_size + bss_size,
		       proc);
		
		total_text += text_size;
		total_data += data_size;
		total_bss += bss_size;

		free(phdr);

		/* Done with proc. */
		(void) fclose(procf);
	}
	/* Done with image. */
	
	fseek(imagef, 4, SEEK_SET);
	fwrite(&num_programs, sizeof(int32_t), 1, imagef);
	fwrite(loc_table, sizeof(uint32_t), num_programs, imagef);

	free(loc_table);

	if (fclose(imagef) == EOF) fatal(image);

	printf("   ------   ------   ------   -------\n");
	printf(" %8ld %8ld %8ld %9ld  total\n",
	       total_text, total_data, total_bss,
	       total_text + total_data + total_bss);
}

void extractsegment(FILE *imagef, char *image, FILE *procf, char *proc,
						long count, off_t *alen)
/* Copy a segment of an executable.  It is padded to a page in image. */
{
	char buf[PAGE_SIZE];

	while (count > 0) {
		bread(imagef, image, buf, sizeof(buf));
		*alen-= sizeof(buf);

		bwrite(procf, proc, buf,
			count < sizeof(buf) ? (size_t) count : sizeof(buf));
		count-= sizeof(buf);
	}
}

void extract_image(char *image)
/* Extract the executables from an image. */
{
	FILE *imagef, *procf;
	off_t len;
	struct stat st;
	struct image_header ihdr;
	Elf32_Ehdr ehdr;
	char buf[PAGE_SIZE];

	if (stat(image, &st) < 0) fatal(image);

	/* Size of the image. */
	len = S_ISREG(st.st_mode) ? st.st_size : -1;

	if ((imagef = fopen(image, "r")) == NULL) fatal(image);

	while (len != 0) {
		/* Extract a program, first sector is an extended header. */
		bread(imagef, image, buf, sizeof(buf));
		len -= sizeof(buf);

		memcpy(&ihdr, buf, sizeof(ihdr));
		ehdr = ihdr.process;

		/* Check header. */
		read_header(ihdr.name, NULL, &ihdr);

		if ((procf = fopen(ihdr.name, "w")) == NULL) fatal(ihdr.name);

		// if (phdr.a_flags & A_PAL) {
		// 	/* A page aligned process contains a header in text. */
		// 	phdr.a_text += sizeof(struct exec);
		// } else {
			// bwrite(procf, ihdr.name, &ihdr.process, sizeof(struct exec));
		// }

		/* Extract text and data segments. */
		// if (phdr.a_flags & A_SEP) {
			// extractexec(imagef, image, procf, ihdr.name, ehdr.a_text, &len);
			// extractexec(imagef, image, procf, ihdr.name, ehdr.a_data, &len);
		// }
		// else {
		// 	extractexec(imagef, image, procf, ihdr.name,
		// 		phdr.a_text + phdr.a_data, &len);
		// }

		if (fclose(procf) == EOF) fatal(ihdr.name);
	}
}

static int rawfd;	/* File descriptor to open device. */
static char *rawdev;	/* Name of device. */

void readblock(off_t blk, char *buf, int block_size)
/* For rawfs, so that it can read blocks. */
{
	int n;

	if (lseek(rawfd, blk * block_size, SEEK_SET) < 0
		|| (n= read(rawfd, buf, block_size)) < 0
	) fatal(rawdev);

	if (n < block_size) {
		fprintf(stderr, "mkimage: Unexpected EOF on %s\n", rawdev);
		exit(1);
	}
}

void writeblock(off_t blk, char *buf, int block_size)
/* Add a function to write blocks for local use. */
{
	if (lseek(rawfd, blk * block_size, SEEK_SET) < 0
		|| write(rawfd, buf, block_size) < 0
	) fatal(rawdev);
}

void usage(void)
{
	fprintf(stderr,
	  "Usage: mkimage -i(mage) image kernel mm fs ... init\n"
	  "       mkimage -(e)x(tract) image\n");
	exit(1);
}

int isoption(char *option, char *test)
/* Check if the option argument is equals "test".  Also accept -i as short
 * for -image, and the special case -x for -extract.
 */
{
	if (strcmp(option, test) == 0) return 1;
	if (option[0] != '-' && strlen(option) != 2) return 0;
	if (option[1] == test[1]) return 1;
	if (option[1] == 'x' && test[1] == 'e') return 1;
	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2) usage();

	if (argc >= 4 && isoption(argv[1], "-image")) {
		make_image(argv[2], argv + 3, argc - 3);
	} else if (argc == 3 && isoption(argv[1], "-extract")) {
		extract_image(argv[2]);
	} else {
		usage();
	}
	exit(0);
}
