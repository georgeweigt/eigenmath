#include "defs.h"

void
eval_expcos(struct atom *p1)
{
	push(cadr(p1));
	eval();
	expcos();
}

void
expcos(void)
{
	struct atom *p1;

	p1 = pop();

	push(imaginaryunit);
	push(p1);
	multiply();
	expfunc();
	push_rational(1, 2);
	multiply();

	push(imaginaryunit);
	negate();
	push(p1);
	multiply();
	expfunc();
	push_rational(1, 2);
	multiply();

	add();
}
