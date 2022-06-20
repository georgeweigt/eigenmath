#include "defs.h"

void
eval_roots(struct atom *p1)
{
	push(cadr(p1));
	eval();

	p1 = cddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		eval();
	} else
		push_symbol(X_LOWER);

	roots();
}

void
roots(void)
{
	int h, i, n;
	struct atom *C, *F, *P, *R, *X;

	X = pop();
	P = pop();

	h = tos;

	factorpoly_coeffs(P, X); // put coeffs on stack

	F = symbol(NIL);

	while (tos - h > 1) {

		C = pop(); // leading coeff

		if (iszero(C))
			continue;

		// divide through by C

		for (i = h; i < tos; i++) {
			push(stack[i]);
			push(C);
			divide();
			stack[i] = pop();
		}

		push_integer(1); // leading coeff

		if (factorpoly_root(h) == 0)
			break;

		R = pop();

		push(R);
		push(F);
		cons();
		F = pop();

		factorpoly_divide(h, R);

		pop(); // remove leading coeff
	}

	tos = h; // pop all

	n = length(F);

	if (n == 0)
		stop("roots");

	if (n == 1) {
		push(car(F));
		return;
	}

	R = alloc_vector(n);

	for (i = 0; i < n; i++) {
		R->u.tensor->elem[i] = car(F);
		F = cdr(F);
	}

	push(R);
}
