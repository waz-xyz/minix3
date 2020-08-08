#include <lib.h>
/*  swab(3)
 *
 *  Author: Terrence W. Holm          Sep. 1988
 */

void swab(const void *from, void *to, ssize_t count)
{
  register char temp;
  char *cfrom = from;
  char *cto = to;

  count >>= 1;

  while (--count >= 0) {
	temp = *cfrom++;
	*cto++ = *cfrom++;
	*cto++ = temp;
  }
}
