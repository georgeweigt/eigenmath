#include "defs.h"

void
eval_rationalize(void)
{
	push(cadr(p1));
	eval();
	rationalize();
}

void
rationalize(void)
{
	int t = expanding;
	save();
	rationalize_nib();
	restore();
	expanding = t;
}

#undef NUM
#undef DEN

#define NUM p5
#define DEN p6

void
rationalize_nib(void)
{
	int i, h;

	p1 = pop();

	if (istensor(p1)) {
		rationalize_tensor();
		return;
	}

	expanding = 0;

	NUM = p1;
	DEN = one;

	// stop after 10 tries

	for (i = 0; i < 10; i++) {

		if (car(NUM) != symbol(ADD))
			break;

		// compute least common multiple of denominators

		push(cadr(NUM));
		denominator();

		p2 = cddr(NUM);

		while (iscons(p2)) {

			push(car(p2));
			denominator();

			p4 = pop();
			p3 = pop();

			push(p3);
			push(p4);
			multiply();

			push(p3);
			push(p4);
			gcd();

			divide();

			p2 = cdr(p2);
		}

		p3 = pop();

		if (equaln(p3, 1))
			break; // numerator has no denominators

		// update numerator

		expanding = 1;
		h = tos;
		p2 = cdr(NUM);
		while (iscons(p2)) {
			push(car(p2));
			push(p3);
			multiply();
			p2 = cdr(p2);
		}
		add_terms(tos - h);
		NUM = pop();
		expanding = 0;

		// update denominator

		push(DEN);
		push(p3);
		multiply();
		DEN = pop();
	}

	push(NUM);
	push(DEN);
	divide();
}

void
rationalize_tensor(void)
{
	int i, n;

	push(p1);
	copy_tensor();
	p1 = pop();

	n = p1->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(p1->u.tensor->elem[i]);
		rationalize();
		p1->u.tensor->elem[i] = pop();
	}

	push(p1);
}
