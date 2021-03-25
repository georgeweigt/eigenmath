#include "defs.h"

void
eval_expcos(void)
{
	push(cadr(p1));
	eval();
	expcos();
}

void
expcos(void)
{
	save();

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

	restore();
}
