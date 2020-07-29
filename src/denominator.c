#include "defs.h"

void
eval_denominator(void)
{
	push(cadr(p1));
	eval();
	denominator();
}

void
denominator(void)
{
	save();

	p1 = pop();
	p2 = one;

	while (cross_expr(p1)) {
		p0 = pop();
		push(p0);
		push(p1);
		cancel_factor();
		p1 = pop();
		push(p0);
		push(p2);
		cancel_factor();
		p2 = pop();
	}

	push(p2);

	restore();
}
