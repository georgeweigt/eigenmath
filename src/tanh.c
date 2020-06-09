#include "defs.h"

void
eval_tanh(void)
{
	push(cadr(p1));
	eval();
	tangenth();
}

void
tangenth(void)
{
	save();
	tangenth_nib();
	restore();
}

void
tangenth_nib(void)
{
	p1 = pop();

	if (isdouble(p1) || isdoublez(p1)) {
		push(p1);
		sineh();
		push(p1);
		cosineh();
		divide();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// tanh(-x) = -tanh(x)

	if (isnegative(p1)) {
		push(p1);
		negate();
		tangenth();
		negate();
		return;
	}

	if (car(p1) == symbol(ARCTANH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(TANH);
	push(p1);
	list(2);
}
