#include "defs.h"

void
eval_lcm(struct atom *p1)
{
	push(cadr(p1));
	eval();

	p1 = cddr(p1);

	while (iscons(p1)) {
		push(car(p1));
		eval();
		lcm();
		p1 = cdr(p1);
	}
}

void
lcm(void)
{
	struct atom *p1, *p2;

	p2 = pop();
	p1 = pop();

	push(p1);
	push(p2);
	gcd();

	push(p1);
	divide();

	push(p2);
	divide();

	reciprocate();
}
