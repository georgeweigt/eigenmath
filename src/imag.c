#include "defs.h"

void
eval_imag(void)
{
	push(cadr(p1));
	eval();
	imag();
}

void
imag(void)
{
	save();
	rect();
	p1 = pop();
	push_rational(-1, 2);
	push(imaginaryunit);
	push(p1);
	push(p1);
	conjugate();
	subtract();
	multiply_factors(3);
	restore();
}
