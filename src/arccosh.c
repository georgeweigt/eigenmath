#include "defs.h"

void
eval_arccosh(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	arccosh();
	expanding = t;
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
	double d;

	p1 = pop();

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		if (d >= 1.0) {
			push_double(acosh(d));
			return;
		}
	}

	// arccosh(z) = log(sqrt(z^2 - 1) + z)

	if (isdouble(p1) || isdoublez(p1)) {
		push(p1);
		push(p1);
		multiply();
		push_double(-1.0);
		add();
		sqrtfunc();
		push(p1);
		add();
		logfunc();
		return;
	}

	if (isplusone(p1)) {
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
