#include "defs.h"

#undef NUM
#undef DEN
#undef R
#undef T

#define NUM p3
#define DEN p4
#define R p5
#define T p6

void
eval_simplify(void)
{
	push(cadr(p1));
	eval();
	simplify();
}

void
simplify(void)
{
	save();
	simplify_nib();
	restore();
}

void
simplify_nib(void)
{
	int h, i, n;

	p1 = pop();

	if (istensor(p1)) {
		push(p1);
		copy_tensor();
		p1 = pop();
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			simplify();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	h = tos;
	push(car(p1));
	p1 = cdr(p1);

	while (iscons(p1)) {
		push(car(p1));
		simplify();
		p1 = cdr(p1);
	}

	list(tos - h);
	eval();
	p1 = pop();
	push(p1);

	if (!iscons(p1))
		return;

	simplify_pass1();
	simplify_pass2();
}

void
simplify_pass1(void)
{
	p1 = pop();

	if (car(p1) == symbol(ADD)) {
		push(p1);
		rationalize();
		T = pop();
		if (car(T) == symbol(ADD)) {
			push(p1); // no change
			return;
		}
	} else
		T = p1;

	push(T);
	numerator();
	NUM = pop();

	push(T);
	denominator();
	eval(); // to expand denominator
	DEN = pop();

	// if DEN is a sum then rationalize it

	if (car(DEN) == symbol(ADD)) {
		push(DEN);
		rationalize();
		T = pop();
		if (car(T) != symbol(ADD)) {
			// update NUM
			push(T);
			denominator();
			eval(); // to expand denominator
			push(NUM);
			multiply();
			NUM = pop();
			// update DEN
			push(T);
			numerator();
			DEN = pop();
		}
	}

	// are NUM and DEN congruent sums?

	if (car(NUM) != symbol(ADD) || car(DEN) != symbol(ADD) || length(NUM) != length(DEN)) {
		// no, but NUM over DEN might be simpler than p1
		push(NUM);
		push(DEN);
		divide();
		T = pop();
		if (complexity(T) < complexity(p1))
			p1 = T;
		push(p1);
		return;
	}

	push(cadr(NUM)); // push first term of numerator
	push(cadr(DEN)); // push first term of denominator
	divide();

	R = pop(); // provisional ratio

	push(R);
	push(DEN);
	multiply();

	push(NUM);
	subtract();

	T = pop();

	if (iszero(T))
		p1 = R;

	push(p1);
}

void
simplify_pass2(void)
{
	p1 = pop();

	push(p1);
	circexp();
	rationalize();
	eval(); // to normalize
	p2 = pop();

	if (complexity(p2) < complexity(p1))
		p1 = p2;

	push(p1);
}
