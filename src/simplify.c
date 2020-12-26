#include "defs.h"

void
eval_simplify(void)
{
	push(cadr(p1));
	eval();
	simplify();
}

void
simplify(void)
{
	save();
	simplify_nib();
	restore();
}

void
simplify_nib(void)
{
	int i, n;

	p1 = pop();

	if (istensor(p1)) {
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			simplify();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	rationalize();
	eval(); // to normalize
	p1 = pop();

	push(p1);
	circexp();
	p2 = pop();

	if (complexity(p2) < complexity(p1))
		p1 = p2;

	push(p1);
}
