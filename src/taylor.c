#include "defs.h"

void
eval_taylor(void)
{
	p1 = cdr(p1);	// f
	push(car(p1));
	eval();

	p1 = cdr(p1);	// x
	push(car(p1));
	eval();

	p1 = cdr(p1);	// n
	push(car(p1));
	eval();

	p1 = cdr(p1);	// a
	push(car(p1));
	eval();

	p2 = pop();
	if (p2 == symbol(NIL))
		push_integer(0); // default expansion point
	else
		push(p2);

	taylor();
}

void
taylor(void)
{
	save();
	taylor_nib();
	restore();
}

#undef F
#undef X
#undef N
#undef A
#undef C

#define F p1
#define X p2
#define N p3
#define A p4
#define C p5

void
taylor_nib(void)
{
	int h, i, k;

	A = pop();
	N = pop();
	X = pop();
	F = pop();

	push(N);
	k = pop_integer();
	if (k == ERR)
		stop("taylor: 3rd arg not numeric or out of range");

	h = tos;

	push(F);	// f(a)
	push(X);
	push(A);
	subst();
	eval();

	C = one;

	for (i = 1; i <= k; i++) {

		push(F);	// f = f'
		push(X);
		derivative();
		F = pop();

		if (iszero(F))
			break;

		if (car(F) == symbol(DERIVATIVE))
			stop("taylor: 1st arg not differentiable");

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
}
