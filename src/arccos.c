#include "defs.h"

void
eval_arccos(void)
{
	push(cadr(p1));
	eval();
	arccos();
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
	int n;

	p1 = pop();

	if (isdouble(p1)) {
		push_double(acos(p1->u.d));
		return;
	}

	// arccos(z) = -i log(z + sqrt(z^2 - 1))

	if (isdoublez(p1)) {
		push(imaginaryunit);
		negate();
		push(p1);
		push(p1);
		push(p1);
		multiply();
		push_double(-1.0);
		add();
		push_rational(1, 2);
		power();
		add();
		logarithm();
		multiply();
		return;
	}

	// if p1 == 1/sqrt(2) then return 1/4*pi (45 degrees)

	if (isoneoversqrttwo(p1)) {
		push_rational(1, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	// if p1 == -1/sqrt(2) then return 3/4*pi (135 degrees)

	if (isminusoneoversqrttwo(p1)) {
		push_rational(3, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	if (!isrational(p1)) {
		push_symbol(ARCCOS);
		push(p1);
		list(2);
		return;
	}

	push(p1);
	push_integer(2);
	multiply();
	n = pop_integer();

	switch (n) {

	case -2:
		push_symbol(PI);
		break;

	case -1:
		push_rational(2, 3);
		push_symbol(PI);
		multiply();
		break;

	case 0:
		push_rational(1, 2);
		push_symbol(PI);
		multiply();
		break;

	case 1:
		push_rational(1, 3);
		push_symbol(PI);
		multiply();
		break;

	case 2:
		push_integer(0);
		break;

	default:
		push_symbol(ARCCOS);
		push(p1);
		list(2);
		break;
	}
}
