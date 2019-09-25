#include "defs.h"

void
eval_denominator(void)
{
	push(cadr(p1));
	eval();
	denominator();
}

// only do rationalize() at top and bottom levels to prevent infinite recursion

void
denominator(void)
{
	save();
	p1 = pop();
	push(p1);
	if (car(p1) == symbol(ADD))
		rationalize();
	denominator1();
	restore();
}

void
denominator1(void)
{
	save();
	denominator1_nib();
	restore();
}

void
denominator1_nib(void)
{
	int h;

	p1 = pop();

	if (p1->k == RATIONAL) {
		push_rational_number(MPLUS, mcopy(p1->u.q.b), mint(1));
		return;
	}

	if (car(p1) == symbol(POWER) && isnegativeterm(caddr(p1))) {
		push(p1);
		reciprocate();
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		h = tos;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			denominator1();
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
		return;
	}

	push(one);
}
