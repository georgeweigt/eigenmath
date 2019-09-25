#include "defs.h"

void
eval_numerator(void)
{
	push(cadr(p1));
	eval();
	numerator();
}

// only do rationalize() at top level to prevent infinite recursion

void
numerator(void)
{
	save();
	p1 = pop();
	push(p1);
	if (car(p1) == symbol(ADD))
		rationalize();
	numerator1();
	restore();
}

void
numerator1(void)
{
	save();
	numerator1_nib();
	restore();
}

void
numerator1_nib(void)
{
	int h;

	p1 = pop();

	if (p1->k == RATIONAL) {
		push_rational_number(p1->sign, mcopy(p1->u.q.a), mint(1));
		return;
	}

	if (car(p1) == symbol(POWER) && isnegativeterm(caddr(p1))) {
		push(one);
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		h = tos;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			numerator1();
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
		return;
	}

	push(p1); // double, sum, etc
}
