#include "defs.h"

void
eval_quotient(void)
{
	push(cadr(p1));			// 1st arg, p(x)
	eval();

	push(caddr(p1));		// 2nd arg, q(x)
	eval();

	push(cadddr(p1));		// 3rd arg, x
	eval();

	p1 = pop();			// default x
	if (p1 == symbol(NIL))
		p1 = symbol(X_LOWER);
	push(p1);

	divpoly();
}

//	Divide polynomials
//
//	Input:		tos-3		Dividend
//
//			tos-2		Divisor
//
//			tos-1		x
//
//	Output:		tos-1		Quotient

#undef DIVIDEND
#undef DIVISOR
#undef X
#undef Q
#undef QQUOTIENT

#define DIVIDEND p1
#define DIVISOR p2
#define X p3
#define Q p4
#define QQUOTIENT p5

void
divpoly(void)
{
	int h, i, m, n, x;
	struct atom **dividend, **divisor;

	save();

	X = pop();
	DIVISOR = pop();
	DIVIDEND = pop();

	h = tos;

	dividend = stack + tos;

	push(DIVIDEND);
	push(X);
	m = coeff() - 1;	// m is dividend's power

	divisor = stack + tos;

	push(DIVISOR);
	push(X);
	n = coeff() - 1;	// n is divisor's power

	x = m - n;

	push_integer(0);
	QQUOTIENT = pop();

	while (x >= 0) {

		push(dividend[m]);
		push(divisor[n]);
		divide();
		Q = pop();

		for (i = 0; i <= n; i++) {
			push(dividend[x + i]);
			push(divisor[i]);
			push(Q);
			multiply();
			subtract();
			dividend[x + i] = pop();
		}

		push(QQUOTIENT);
		push(Q);
		push(X);
		push_integer(x);
		power();
		multiply();
		add();
		QQUOTIENT = pop();

		m--;
		x--;
	}

	tos = h;

	push(QQUOTIENT);

	restore();
}
