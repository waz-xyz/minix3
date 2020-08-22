/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include <string.h>

void *memcpy(void *dest, const void *src, size_t n)
{
	char *p1 = dest;
	const char *p2 = src;

	if (n) {
		n++;
		while (--n > 0) {
			*p1++ = *p2++;
		}
	}
	return dest;
}
