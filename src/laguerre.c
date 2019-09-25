/* Laguerre function

The computation uses the following recurrence relation.

	L(x,0,k) = 1

	L(x,1,k) = -x + k + 1

	n*L(x,n,k) = (2*(n-1)+1-x+k)*L(x,n-1,k) - (n-1+k)*L(x,n-2,k)

In the "for" loop i = n-1 so the recurrence relation becomes

	(i+1)*L(x,n,k) = (2*i+1-x+k)*L(x,n-1,k) - (i+k)*L(x,n-2,k)
*/

#include "defs.h"

void
eval_laguerre(void)
{
	// 1st arg

	push(cadr(p1));
	eval();

	// 2nd arg

	push(caddr(p1));
	eval();

	// 3rd arg

	push(cadddr(p1));
	eval();

	p2 = pop();
	if (p2 == symbol(NIL))
		push_integer(0);
	else
		push(p2);

	laguerre();
}

#undef X
#undef N
#undef K
#undef Y
#undef Y0
#undef Y1

#define X p1
#define N p2
#define K p3
#define Y p4
#define Y0 p5
#define Y1 p6

void
laguerre(void)
{
	int n;
	save();

	K = pop();
	N = pop();
	X = pop();

	push(N);
	n = pop_integer();

	if (n < 0) {
		push_symbol(LAGUERRE);
		push(X);
		push(N);
		push(K);
		list(4);
		restore();
		return;
	}

	if (issymbol(X))
		laguerre2(n);
	else {
		Y = X;			// do this when X is an expr
		X = symbol(SPECX);
		laguerre2(n);
		X = Y;
		push_symbol(SPECX);
		push(X);
		subst();
		eval();
	}

	restore();
}

void
laguerre2(int n)
{
	int i;

	push_integer(1);
	push_integer(0);

	Y1 = pop();

	for (i = 0; i < n; i++) {

		Y0 = Y1;

		Y1 = pop();

		push_integer(2 * i + 1);
		push(X);
		subtract();
		push(K);
		add();
		push(Y1);
		multiply();

		push_integer(i);
		push(K);
		add();
		push(Y0);
		multiply();

		subtract();

		push_integer(i + 1);
		divide();
	}
}
