#include "defs.h"

void
eval_isprime(void)
{
	push(cadr(p1));
	eval();
	p1 = pop();
	if (isposint(p1) && mprime(p1->u.q.a))
		push_integer(1);
	else
		push_integer(0);
}
