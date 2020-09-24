#include <lib.h>
#define dup	_dup
#define fcntl	_fcntl
#include <fcntl.h>
#include <unistd.h>

PUBLIC int dup(int fd)
{
	return fcntl(fd, F_DUPFD, 0);
}
