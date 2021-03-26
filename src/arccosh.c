#include "defs.h"

void
eval_arccosh(void)
{
	push(cadr(p1));
	eval();
	arccosh();
}

void
arccosh(void)
{
	save();
	arccosh_nib();
	restore();
}

void
arccosh_nib(void)
{
	p1 = pop();

	if (isdouble(p1) && p1->u.d >= 1.0) {
		push_double(acosh(p1->u.d));
		return;
	}

	// arccosh(z) = log(z + (z^2 - 1)^(1/2))

	if (isdouble(p1) || isdoublez(p1)) {
		push(p1);
		push(p1);
		multiply();
		push_double(-1.0);
		add();
		push_rational(1, 2);
		power();
		push(p1);
		add();
		logfunc();
		return;
	}

	if (equaln(p1, 1)) {
		push_integer(0);
		return;
	}

	if (car(p1) == symbol(COSH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCCOSH);
	push(p1);
	list(2);
}
