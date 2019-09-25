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
	int n;

	save();

	p2 = pop();
	p1 = pop();

	if (iszero(p2))
		stop("divide by zero");

	if (!isnum(p1) || !isnum(p2)) {
		push_symbol(MOD);
		push(p1);
		push(p2);
		list(3);
		restore();
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		n = pop_integer();
		if (n == ERR)
			stop("mod function: cannot convert float value to integer");
		push_integer(n);
		p1 = pop();
	}

	if (isdouble(p2)) {
		push(p2);
		n = pop_integer();
		if (n == ERR)
			stop("mod function: cannot convert float value to integer");
		push_integer(n);
		p2 = pop();
	}

	if (!isinteger(p1) || !isinteger(p2))
		stop("mod function: integer arguments expected");

	push_rational_number(p1->sign, mmod(p1->u.q.a, p2->u.q.a), mint(1));

	restore();
}
