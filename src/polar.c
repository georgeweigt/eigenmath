#include "defs.h"

void
eval_polar(void)
{
	push(cadr(p1));
	eval();
	polar();
}

void
polar(void)
{
	save();
	polar_nib();
	restore();
}

void
polar_nib(void)
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
			polar();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	mag();
	push(imaginaryunit);
	push(p1);
	arg();
	multiply();
	exponential();
	multiply();
}
