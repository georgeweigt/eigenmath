#include "defs.h"

void
eval_mod(void)
{
	int t = expanding;
	expanding = 1;
	push(cadr(p1));
	eval();
	push(caddr(p1));
	eval();
	modfunc();
	expanding = t;
}

void
modfunc(void)
{
	save();
	modfunc_nib();
	restore();
}

void
modfunc_nib(void)
{
	double d1, d2;

	p2 = pop();
	p1 = pop();

	if (!isnum(p1) || !isnum(p2) || iszero(p2)) {
		push_symbol(MOD);
		push(p1);
		push(p2);
		list(3);
		return;
	}

	if (isrational(p1) && isrational(p2)) {
		modfunc_rationals();
		return;
	}

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	push_double(fmod(d1, d2));
}

void
modfunc_rationals(void)
{
	if (isinteger(p1) && isinteger(p2)) {
		push_rational_number(p1->sign, mmod(p1->u.q.a, p2->u.q.a), mint(1));
		return;
	}
	push(p1);
	push(p1);
	push(p2);
	divide();
	absfunc();
	floorfunc();
	push(p2);
	multiply();
	if (p1->sign == p2->sign)
		negate();
	add();
}
