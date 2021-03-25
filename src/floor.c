#include "defs.h"

void
eval_floor(void)
{
	push(cadr(p1));
	eval();
	floorfunc();
}

void
floorfunc(void)
{
	save();
	floorfunc_nib();
	restore();
}

void
floorfunc_nib(void)
{
	double d;

	p1 = pop();

	if (!isnum(p1)) {
		push_symbol(FLOOR);
		push(p1);
		list(2);
		return;
	}

	if (isdouble(p1)) {
		d = floor(p1->u.d);
		push_double(d);
		return;
	}

	if (isinteger(p1)) {
		push(p1);
		return;
	}

	push_rational_number(p1->sign, mdiv(p1->u.q.a, p1->u.q.b), mint(1));

	if (p1->sign == MMINUS) {
		push_integer(-1);
		add();
	}
}
