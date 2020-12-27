#include "defs.h"

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
	int i, h, n;

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

	if (car(p1) == symbol(ADD)) {
		// simplify each term
		h = tos;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			simplify_expr();
			p1 = cdr(p1);
		}
		add_terms(tos - h);
		p1 = pop();
		if (car(p1) == symbol(ADD)) {
			push(p1);
			simplify_expr(); // try rationalizing
			p1 = pop();
		}
		push(p1);
		return;
	}

	// p1 is a term (factor or product of factors)

	push(p1);
	simplify_expr();
}

void
simplify_expr(void)
{
	save();
	simplify_expr_nib();
	simplify_trig();
	restore();
}

#undef NUM
#undef DEN
#undef R
#undef T

#define NUM p2
#define DEN p3
#define R p4
#define T p5

void
simplify_expr_nib(void)
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
simplify_trig(void)
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
