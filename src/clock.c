#include "defs.h"

void
eval_clock(void)
{
	push(cadr(p1));
	eval();
	clockform();
}

void
clockform(void)
{
	save();
	p1 = pop();
	push(p1);
	mag();
	push_integer(-1);
	push(p1);
	arg();
	push_symbol(PI);
	divide();
	power();
	multiply();
	restore();
}
