#include "defs.h"

void
eval_exp(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	expfunc();
	expanding = t;
}

void
expfunc(void)
{
	push_symbol(EXP1);
	swap();
	power();
}
