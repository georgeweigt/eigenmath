#include "defs.h"

void
eval_expsin(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	expsin();
	expanding = t;
}

void
expsin(void)
{
	save();

	p1 = pop();

	push(imaginaryunit);
	push(p1);
	multiply();
	expfunc();
	push(imaginaryunit);
	divide();
	push_rational(1, 2);
	multiply();

	push(imaginaryunit);
	negate();
	push(p1);
	multiply();
	expfunc();
	push(imaginaryunit);
	divide();
	push_rational(1, 2);
	multiply();

	subtract();

	restore();
}
