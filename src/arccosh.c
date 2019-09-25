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
		logarithm();
		return;
	}

	if (equaln(p1, 1)) {
		push(zero);
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
