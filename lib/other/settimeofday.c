/*
settimeofday.c
*/

#define stime _stime

#include <sys/time.h>
#include <time.h>

int settimeofday(const struct timeval *tp, const void *tzp)
{
	time_t top = tp->tv_sec;

	/* Ignore time zones */
	return stime(&top);
}
