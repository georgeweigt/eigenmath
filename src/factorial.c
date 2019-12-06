#include "defs.h"

void
eval_factorial(void)
{
	push(cadr(p1));
	eval();
	factorial();
}

void
factorial(void)
{
	save();
	factorial_nib();
	restore();
}

void
factorial_nib(void)
{
	int n;
	p1 = pop();
	push(p1);
	n = pop_integer();
	if (n < 0) {
		push_symbol(FACTORIAL);
		push(p1);
		list(2);
		return;
	}
	bignum_factorial(n);
	if (isdouble(p1))
		bignum_float();
}
