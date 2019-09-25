#include "defs.h"

void
eval_bessely(void)
{
	push(cadr(p1));
	eval();
	push(caddr(p1));
	eval();
	bessely();
}

void
bessely(void)
{
	save();
	bessely_nib();
	restore();
}

#undef X
#undef N

#define X p1
#define N p2

void
bessely_nib(void)
{
	double d;
	int n;

	N = pop();
	X = pop();

	push(N);
	n = pop_integer();

	if (isdouble(X) && n != ERR) {
		d = yn(n, X->u.d); // math.h
		push_double(d);
		return;
	}

	if (isnegativeterm(N)) {
		push_integer(-1);
		push(N);
		power();
		push_symbol(BESSELY);
		push(X);
		push(N);
		negate();
		list(3);
		multiply();
		return;
	}

	push_symbol(BESSELY);
	push(X);
	push(N);
	list(3);
}
