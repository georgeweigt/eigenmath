#include "defs.h"

void
eval_denominator(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	denominator();
	expanding = t;
}

void
denominator(void)
{
	save();
	denominator_nib();
	restore();
}

void
denominator_nib(void)
{
	p1 = pop();

	if (isrational(p1)) {
		push_rational_number(MPLUS, mcopy(p1->u.q.b), mint(1));
		return;
	}

	p2 = one; // denominator

	while (cross_expr(p1)) {

		p0 = pop(); // p0 is a denominator

		push(p0); // cancel in orig expr
		push(p1);
		cancel_factor();
		p1 = pop();

		push(p0); // update denominator
		push(p2);
		cancel_factor();
		p2 = pop();
	}

	push(p2);
}
