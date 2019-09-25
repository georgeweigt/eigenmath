#include "defs.h"

#undef POLY
#undef X
#undef A
#undef B
#undef C
#undef Y

#define POLY p1
#define X p2
#define A p3
#define B p4
#define C p5
#define Y p6

void
eval_roots(void)
{
	// A == B -> A - B

	p2 = cadr(p1);

	if (car(p2) == symbol(SETQ) || car(p2) == symbol(TESTEQ)) {
		push(cadr(p2));
		eval();
		push(caddr(p2));
		eval();
		subtract();
	} else {
		push(p2);
		eval();
		p2 = pop();
		if (car(p2) == symbol(SETQ) || car(p2) == symbol(TESTEQ)) {
			push(cadr(p2));
			eval();
			push(caddr(p2));
			eval();
			subtract();
		} else
			push(p2);
	}

	// 2nd arg, x

	push(caddr(p1));
	eval();
	p2 = pop();
	if (p2 == symbol(NIL))
		guess();
	else
		push(p2);

	p2 = pop();
	p1 = pop();

	if (!ispoly(p1, p2))
		stop("roots: 1st argument is not a polynomial");

	push(p1);
	push(p2);

	roots();
}

void
roots(void)
{
	int h, i, n;
	h = tos - 2;
	roots2();
	n = tos - h;
	if (n == 0)
		stop("roots: the polynomial is not factorable, try nroots");
	if (n == 1)
		return;
	sort(n);
	save();
	p1 = alloc_tensor(n);
	p1->u.tensor->ndim = 1;
	p1->u.tensor->dim[0] = n;
	for (i = 0; i < n; i++)
		p1->u.tensor->elem[i] = stack[h + i];
	tos = h;
	push(p1);
	restore();
}

void
roots2(void)
{
	save();

	p2 = pop();
	p1 = pop();

	push(p1);
	push(p2);
	factorpoly();

	p1 = pop();

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			push(p2);
			roots3();
			p1 = cdr(p1);
		}
	} else {
		push(p1);
		push(p2);
		roots3();
	}

	restore();
}

void
roots3(void)
{
	save();
	p2 = pop();
	p1 = pop();
	if (car(p1) == symbol(POWER) && ispoly(cadr(p1), p2) && isposint(caddr(p1))) {
		push(cadr(p1));
		push(p2);
		mini_solve();
	} else if (ispoly(p1, p2)) {
		push(p1);
		push(p2);
		mini_solve();
	}
	restore();
}

//	Input:		stack[tos - 2]		polynomial
//
//			stack[tos - 1]		dependent symbol
//
//	Output:		stack			roots on stack
//
//						(input args are popped first)

void
mini_solve(void)
{
	int n;

	save();

	X = pop();
	POLY = pop();

	push(POLY);
	push(X);

	n = coeff();

	// AX + B, X = -B/A

	if (n == 2) {
		A = pop();
		B = pop();
		push(B);
		push(A);
		divide();
		negate();
		restore();
		return;
	}

	// AX^2 + BX + C, X = (-B +/- (B^2 - 4AC)^(1/2)) / (2A)

	if (n == 3) {
		A = pop();
		B = pop();
		C = pop();
		push(B);
		push(B);
		multiply();
		push_integer(4);
		push(A);
		multiply();
		push(C);
		multiply();
		subtract();
		push_rational(1, 2);
		power();
		Y = pop();
		push(Y);			// 1st root
		push(B);
		subtract();
		push(A);
		divide();
		push_rational(1, 2);
		multiply();
		push(Y);			// 2nd root
		push(B);
		add();
		negate();
		push(A);
		divide();
		push_rational(1, 2);
		multiply();
		restore();
		return;
	}

	tos -= n;

	restore();
}
