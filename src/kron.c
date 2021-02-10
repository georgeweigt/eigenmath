#include "defs.h"

void
eval_kron(void)
{
	push(cadr(p1));
	eval();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		eval();
		kron();
		p1 = cdr(p1);
	}
}

void
kron(void)
{
	save();
	kron_nib();
	restore();
}

void
kron_nib(void)
{
	p2 = pop();
	p1 = pop();

	push_symbol(NIL); // under construction
}
