#include "defs.h"

void
eval_numerator(void)
{
	push(cadr(p1));
	eval();
	numerator();
}

void
numerator(void)
{
	save();
	numerator_nib();
	restore();
}

void
numerator_nib(void)
{
	p1 = pop();

	if (isrational(p1)) {
		push_rational_number(p1->sign, mcopy(p1->u.q.a), mint(1));
		return;
	}

	while (cross_expr(p1)) {
		push(p1);
		cancel_factor();
		p1 = pop();
	}

	push(p1);
}
