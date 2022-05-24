#include "defs.h"

#undef F
#undef X
#undef A
#undef C

#define F p3
#define X p4
#define A p5
#define C p6

void
eval_taylor(void)
{
	int h, i, n, t;

	t = expanding;
	expanding = 1;

	push(cadr(p1));
	eval();
	F = pop();

	push(caddr(p1));
	eval();
	X = pop();

	push(cadddr(p1));
	eval();
	n = pop_integer();

	if (iscons(cddddr(p1))) {
		push(caddddr(p1));
		eval();
	} else
		push_integer(0); // default expansion point

	A = pop();

	h = tos;

	push(F);	// f(a)
	push(X);
	push(A);
	subst();
	eval();

	C = one;

	for (i = 1; i <= n; i++) {

		push(F);	// f = f'
		push(X);
		derivative();
		F = pop();

		if (iszero(F))
			break;

		push(C);	// c = c * (x - a)
		push(X);
		push(A);
		subtract();
		multiply();
		C = pop();

		push(F);	// f(a)
		push(X);
		push(A);
		subst();
		eval();

		push(C);
		multiply();
		push_integer(i);
		factorial();
		divide();
	}

	add_terms(tos - h);

	expanding = t;
}
