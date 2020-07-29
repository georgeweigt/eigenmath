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

	p1 = pop();

	while (cross_expr(p1)) {
		push(p1);
		cancel_factor();
		p1 = pop();
	}

	push(p1);

	restore();
}
