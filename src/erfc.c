#include "defs.h"

void
eval_erfc(struct atom *p1)
{
	push(cadr(p1));
	eval();
	erfcfunc();
}

void
erfcfunc(void)
{
	double d;
	struct atom *p1;

	p1 = pop();

	if (isdouble(p1)) {
		d = erfc(p1->u.d);
		push_double(d);
		return;
	}

	push_symbol(ERFC);
	push(p1);
	list(2);
}
