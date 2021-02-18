#include "defs.h"

void
eval_noexpand(void)
{
	int t;

	t = expanding;
	expanding = 0;

	push(cadr(p1));
	eval();

	expanding = t;
}
