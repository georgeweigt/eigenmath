#include "defs.h"

void
eval_tanh(void)
{
	push(cadr(p1));
	eval();
	stanh();
}

void
stanh(void)
{
	save();
	stanh_nib();
	restore();
}

void
stanh_nib(void)
{
	p1 = pop();

	if (isdouble(p1) || isdoublez(p1)) {
		push(p1);
		ssinh();
		push(p1);
		scosh();
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
		stanh();
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
