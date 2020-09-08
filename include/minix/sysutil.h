#ifndef _EXTRALIB_H
#define _EXTRALIB_H

/* Extra system library definitions to support device drivers and servers.
 *
 * Created:
 *	Mar 15, 2004 by Jorrit N. Herder
 *
 * Changes:
 *	May 31, 2005: added printf, kputc (relocated from syslib)
 *	May 31, 2005: added getuptime
 *	Mar 18, 2005: added tickdelay
 *	Oct 01, 2004: added env_parse, env_prefix, env_panic
 *	Jul 13, 2004: added fkey_ctl
 *	Apr 28, 2004: added report, panic 
 *	Mar 31, 2004: setup like other libraries, such as syslib
 */

/*==========================================================================* 
 * Miscellaneous helper functions.
 *==========================================================================*/ 

/* Environment parsing return values. */
#define EP_BUF_SIZE	128	/* local buffer for env value */
#define EP_UNSET	0	/* variable not set */
#define EP_OFF		1	/* var = off */
#define EP_ON		2	/* var = on (or field left blank) */
#define EP_SET		3	/* var = 1:2:3 (nonblank field) */
#define EP_EGETKENV	4	/* sys_getkenv() failed ... */

void env_setargs(int argc, char *argv[]);
int env_get_param(char *key, char *value, int max_size);
int env_prefix(char *env, char *prefix);
void env_panic(char *key);
int env_parse(char *env, char *fmt, int field, long *param, long min, long max);

#define fkey_map(fkeys, sfkeys)		fkey_ctl(FKEY_MAP, (fkeys), (sfkeys))
#define fkey_unmap(fkeys, sfkeys)	fkey_ctl(FKEY_UNMAP, (fkeys), (sfkeys))
#define fkey_events(fkeys, sfkeys)	fkey_ctl(FKEY_EVENTS, (fkeys), (sfkeys))
int fkey_ctl(int req, int *fkeys, int *sfkeys);

int printf(const char *fmt, ...);
void kputc(int c);
void report(char *who, char *mess, int num);
void panic(char *who, char *mess, int num);
int getuptime(clock_t *ticks);
int tickdelay(clock_t ticks);

#endif /* _EXTRALIB_H */
