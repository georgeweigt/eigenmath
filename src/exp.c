#include "defs.h"

void
eval_exp(struct atom *p1)
{
	push(cadr(p1));
	eval();
	expfunc();
}

void
expfunc(void)
{
	push_symbol(EXP1);
	swap();
	power();
}
