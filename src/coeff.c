#include "defs.h"

// get the coefficient of x^n in polynomial p(x)

void
eval_coeff(struct atom *p1)
{
	struct atom *P, *X, *N;

	push(cadr(p1));
	eval();
	P = pop();

	push(caddr(p1));
	eval();
	X = pop();

	push(cadddr(p1));
	eval();
	N = pop();

	push(P); // divide p by x^n
	push(X);
	push(N);
	power();
	divide();

	push(X); // keep the constant part
	filter();
}

int
coeff(void)
{
	int h;
	struct atom *P, *X, *C;

	X = pop();
	P = pop();

	h = tos;

	for (;;) {

		push(P);
		push(X);
		push_integer(0);
		subst();
		eval();
		C = pop();

		push(C);

		push(P);
		push(C);
		subtract();
		P = pop();

		if (iszero(P))
			break;

		push(P);
		push(X);
		divide();
		P = pop();
	}

	return tos - h;
}
