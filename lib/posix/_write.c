#include <lib.h>
#define write	_write
#include <unistd.h>

PUBLIC ssize_t write(int fd, const void *buffer, size_t nbytes)
{
	message m;

	m.m1_i1 = fd;
	m.m1_i2 = nbytes;
	m.m1_p1 = (char *) buffer;
	return _syscall(FS, WRITE, &m);
}
