#include "defs.h"

void
eval_bessely(struct atom *p1)
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
	int n;
	double d;
	struct atom *X, *N;

	N = pop();
	X = pop();

	if (isdouble(X)) {
		push(N);
		n = pop_integer();
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
