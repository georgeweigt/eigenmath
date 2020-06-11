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
		mod_rationals();
		return;
	}

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	push_double(fmod(d1, d2));
}

void
mod_rationals(void)
{
	uint32_t *a, *b, *q;

	if (isinteger(p1) && isinteger(p2)) {
		push_rational_number(p1->sign, mmod(p1->u.q.a, p2->u.q.a), mint(1));
		return;
	}

	a = mmul(p1->u.q.a, p2->u.q.b);
	b = mmul(p1->u.q.b, p2->u.q.a);

	q = mdiv(a, b);

	mfree(a);
	mfree(b);

	push(p1);
	push_rational_number(p1->sign, q, mint(1));
	push(p2);
	absval();
	multiply();
	subtract();
}
