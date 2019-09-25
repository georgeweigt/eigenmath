// exponential sine function

#include "defs.h"

void
eval_expsin(void)
{
	push(cadr(p1));
	eval();
	expsin();
}

void
expsin(void)
{
	save();

	p1 = pop();

	push(imaginaryunit);
	push(p1);
	multiply();
	exponential();
	push(imaginaryunit);
	divide();
	push_rational(1, 2);
	multiply();

	push(imaginaryunit);
	negate();
	push(p1);
	multiply();
	exponential();
	push(imaginaryunit);
	divide();
	push_rational(1, 2);
	multiply();

	subtract();

	restore();
}
