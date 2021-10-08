#include "defs.h"

void
eval_cosh(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	coshfunc();
	expanding = t;
}

void
coshfunc(void)
{
	save();
	coshfunc_nib();
	restore();
}

void
coshfunc_nib(void)
{
	double d;

	p1 = pop();

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = cosh(d);
		push_double(d);
		return;
	}

	// cosh(z) = 1/2 exp(z) + 1/2 exp(-z)

	if (isdoublez(p1)) {
		push_rational(1, 2);
		push(p1);
		expfunc();
		push(p1);
		negate();
		expfunc();
		add();
		multiply();
		return;
	}

	if (iszero(p1)) {
		push_integer(1);
		return;
	}

	// cosh(-x) = cosh(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		coshfunc();
		return;
	}

	if (car(p1) == symbol(ARCCOSH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(COSH);
	push(p1);
	list(2);
}
