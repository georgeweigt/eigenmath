#include "defs.h"

void
eval_inv(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	inv();
	expanding = t;
}

void
inv(void)
{
	save();
	inv_nib();
	restore();
}

void
inv_nib(void)
{
	p1 = pop();

	if (!istensor(p1)) {
		push(p1);
		reciprocate();
		return;
	}

	if (!issquarematrix(p1))
		stop("square matrix expected");

	push(p1);
	adj();

	push(p1);
	det();

	divide();
}
