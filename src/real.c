#include "defs.h"

void
eval_real(void)
{
	push(cadr(p1));
	eval();
	real();
}

void
real(void)
{
	save();
	real_nib();
	restore();
}

void
real_nib(void)
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
			real();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	rect();
	p1 = pop();
	push(p1);
	push(p1);
	conjfunc();
	add();
	push_rational(1, 2);
	multiply();
}
