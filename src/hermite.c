#include "defs.h"

void
eval_hermite(void)
{
	push(cadr(p1));
	eval();
	push(caddr(p1));
	eval();
	hermite();
}

void
hermite(void)
{
	save();
	hermite_nib();
	restore();
}

#undef X
#undef N
#undef Y1
#undef Y0

#define X p1
#define N p2
#define Y1 p3
#define Y0 p4

// H(x,n) = 2 x H(x,n - 1) - 2 (n - 1) H(x,n - 2)

void
hermite_nib(void)
{
	int i, n;

	N = pop();
	X = pop();

	push(N);
	n = pop_integer();

	if (n == ERR) {
		push_symbol(HERMITE);
		push(X);
		push(N);
		list(3);
		return;
	}

	if (n < 0) {
		push(zero);
		return;
	}

	push(one);

	Y1 = zero;

	for (i = 0; i < n; i++) {

		Y0 = Y1;

		Y1 = pop();

		push(X);
		push(Y1);
		multiply();

		push_integer(i);
		push(Y0);
		multiply();

		subtract();

		push_integer(2);
		multiply();
	}
}
