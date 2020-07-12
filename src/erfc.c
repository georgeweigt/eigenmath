#include "defs.h"

void
eval_erfc(void)
{
	push(cadr(p1));
	eval();
	serfc();
}

void
serfc(void)
{
	save();
	serfc_nib();
	restore();
}

void
serfc_nib(void)
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
