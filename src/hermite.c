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
#undef Y
#undef Y1
#undef Y0

#define X p1
#define N p2
#define Y p3
#define Y1 p4
#define Y0 p5

// uses the recurrence relation H(x,n+1)=2*x*H(x,n)-2*n*H(x,n-1)

void
hermite_nib(void)
{
	int n;

	N = pop();
	X = pop();

	push(N);
	n = pop_integer();

	if (n < 0) {
		push_symbol(HERMITE);
		push(X);
		push(N);
		list(3);
		return;
	}

	if (issymbol(X))
		hermite2(n);
	else {
		Y = X;			// do this when X is an expr
		X = symbol(SPECX);
		hermite2(n);
		X = Y;
		push_symbol(SPECX);
		push(X);
		subst();
		eval();
	}
}

void
hermite2(int n)
{
	int i;

	push_integer(1);
	push_integer(0);

	Y1 = pop();

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
