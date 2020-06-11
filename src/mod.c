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

	if (!isnum(p1) || !isnum(p2)) {
		push_symbol(MOD);
		push(p1);
		push(p2);
		list(3);
		return;
	}

	if (isdouble(p1)) {
		convert_double_to_rational(p1->u.d);
		p1 = pop();
	}

	if (isdouble(p2)) {
		convert_double_to_rational(p2->u.d);
		p2 = pop();
	}

	if (!isinteger(p1) || !isinteger(p2))
		stop("mod: integer arguments expected");

	push_rational_number(p1->sign, mmod(p1->u.q.a, p2->u.q.a), mint(1));
}
