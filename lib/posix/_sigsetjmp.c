#include <lib.h>
#include <sys/sigcontext.h>
#include <setjmp.h>

PUBLIC void siglongjmp(sigjmp_buf env, int val)
{
	longjmp(env, val);
}
