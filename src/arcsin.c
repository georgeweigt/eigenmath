#include "defs.h"

void
eval_arcsin(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	arcsin();
	expanding = t;
}

void
arcsin(void)
{
	save();
	arcsin_nib();
	restore();
}

void
arcsin_nib(void)
{
	p1 = pop();

	if (isdouble(p1)) {
		push_double(asin(p1->u.d));
		return;
	}

	// arcsin(z) = -i log(i z + sqrt(1 - z^2))

	if (isdoublez(p1)) {
		push(imaginaryunit);
		negate();
		push(imaginaryunit);
		push(p1);
		multiply();
		push_double(1.0);
		push(p1);
		push(p1);
		multiply();
		subtract();
		push_rational(1, 2);
		power();
		add();
		logfunc();
		multiply();
		return;
	}

	// arcsin(-x) = -arcsin(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		arcsin();
		negate();
		return;
	}

	// arcsin(1 / sqrt(2)) = 1/4 pi

	if (isoneoversqrttwo(p1)) {
		push_rational(1, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	// arcsin(0) = 0

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// arcsin(1/2) = 1/6 pi

	if (isequalq(p1, 1, 2)) {
		push_rational(1, 6);
		push_symbol(PI);
		multiply();
		return;
	}

	// arcsin(1) = 1/2 pi

	if (isequaln(p1, 1)) {
		push_rational(1, 2);
		push_symbol(PI);
		multiply();
		return;
	}

	push_symbol(ARCSIN);
	push(p1);
	list(2);
}
