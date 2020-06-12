#include "defs.h"

void
eval_mod(void)
{
	push(cadr(p1));
	eval();
	push(caddr(p1));
	eval();
	mod();
}

void
mod(void)
{
	save();
	mod_nib();
	restore();
}

void
mod_nib(void)
{
	p2 = pop();
	p1 = pop();

	if (iszero(p2))
		stop("mod: divide by zero");

	if (isnum(p1) && isnum(p2)) {
		mod_numbers();
		return;
	}

	push_symbol(MOD);
	push(p1);
	push(p2);
	list(3);
}

void
mod_numbers(void)
{
	double d1, d2;

	if (isrational(p1) && isrational(p2)) {
		push(p1);
		push(p1);
		push(p2);
		divide();
		absval();
		sfloor();
		push(p2);
		multiply();
		if (p1->sign == p2->sign)
			negate();
		add();
		return;
	}

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	push_double(fmod(d1, d2));
}
