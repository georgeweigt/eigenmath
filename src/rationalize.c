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
	save();
	rationalize_nib();
	restore();

}

void
rationalize_nib(void)
{
	int i, n;

	p1 = pop();

	if (istensor(p1)) {
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
		return;
	}

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

	push(p1);
	push(p2);
	reciprocate();
	multiply_noexpand();
}
