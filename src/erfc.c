#include "defs.h"

void
eval_erfc(void)
{
	push(cadr(p1));
	eval();
	erfc_symbolic();
}

void
erfc_symbolic(void)
{
	save();
	erfc_symbolic_nib();
	restore();
}

void
erfc_symbolic_nib(void)
{
	double d;

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
