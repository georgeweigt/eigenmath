#include "defs.h"

void
eval_arccos(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	arccos();
	expanding = t;
}

void
arccos(void)
{
	save();
	arccos_nib();
	restore();
}

void
arccos_nib(void)
{
	p1 = pop();

	if (isdouble(p1)) {
		push_double(acos(p1->u.d));
		return;
	}

	// arccos(z) = -i log(z + i sqrt(1 - z^2))

	if (isdoublez(p1)) {
		push_double(1.0);
		push(p1);
		push(p1);
		multiply();
		subtract();
		sqrtfunc();
		push(imaginaryunit);
		multiply();
		push(p1);
		add();
		logfunc();
		push(imaginaryunit);
		multiply();
		negate();
		return;
	}

	// arccos(1 / sqrt(2)) = 1/4 pi

	if (isoneoversqrttwo(p1)) {
		push_rational(1, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(-1 / sqrt(2)) = 3/4 pi

	if (isminusoneoversqrttwo(p1)) {
		push_rational(3, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(0) = 1/2 pi

	if (iszero(p1)) {
		push_rational(1, 2);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(1/2) = 1/3 pi

	if (isequalq(p1, 1 ,2)) {
		push_rational(1, 3);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(1) = 0

	if (isequaln(p1, 1)) {
		push_integer(0);
		return;
	}

	// arccos(-1/2) = 2/3 pi

	if (isequalq(p1, -1, 2)) {
		push_rational(2, 3);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(-1) = pi

	if (isequaln(p1, -1)) {
		push_symbol(PI);
		return;
	}

	push_symbol(ARCCOS);
	push(p1);
	list(2);
}
