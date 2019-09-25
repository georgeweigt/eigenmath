#include "defs.h"

void
eval_conj(void)
{
	push(cadr(p1));
	eval();
	conjugate();
}


void
conjugate(void)
{
	push(imaginaryunit);
	push(imaginaryunit);
	negate();
	subst();
	eval();
}
