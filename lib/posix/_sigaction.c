#include <lib.h>
#define sigaction _sigaction
#include <sys/sigcontext.h>
#include <signal.h>

int __sigreturn(void);

PUBLIC int sigaction(int sig, const struct sigaction *act, struct sigaction *oact)
{
	message m;

	m.m1_i2 = sig;

	/* XXX - yet more type puns because message struct is short of types. */
	m.m1_p1 = (char *)act;
	m.m1_p2 = (char *)oact;
	m.m1_p3 = (char *)__sigreturn;

	return _syscall(MM, SIGACTION, &m);
}
