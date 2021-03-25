#include "defs.h"

void
eval_exp(void)
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
