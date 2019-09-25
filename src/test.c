// If the number of args is odd then the last arg is the default result.

#include "defs.h"

void
eval_test(void)
{
	p1 = cdr(p1);
	while (iscons(p1)) {
		if (cdr(p1) == symbol(NIL)) {
			push(car(p1)); // default case
			eval();
			return;
		}
		push(car(p1));
		evalp();
		p2 = pop();
		if (!iszero(p2)) {
			push(cadr(p1));
			eval();
			return;
		}
		p1 = cddr(p1);
	}
	push(zero);
}

void
eval_testeq(void)
{
	push(cadr(p1));
	eval();

	push(caddr(p1));
	eval();

	p2 = pop();
	p1 = pop();

	// this is for comparing null tensor with scalar zero

	if (iszero(p1)) {
		if (iszero(p2))
			push(one);
		else
			push(zero);
		return;
	}

	if (iszero(p2)) {
		push(zero);
		return;
	}

	// try this first

	if (equal(p1, p2)) {
		push(one);
		return;
	}

	// subtraction might simplify to zero

	push(p1);
	push(p2);
	subtract();

	p1 = pop();

	if (iszero(p1))
		push(one);
	else
		push(zero);
}

// Relational operators expect a numeric result for operand difference.

void
eval_testge(void)
{
	if (cmp_args() >= 0)
		push_integer(1);
	else
		push_integer(0);
}

void
eval_testgt(void)
{
	if (cmp_args() > 0)
		push_integer(1);
	else
		push_integer(0);
}

void
eval_testle(void)
{
	if (cmp_args() <= 0)
		push_integer(1);
	else
		push_integer(0);
}

void
eval_testlt(void)
{
	if (cmp_args() < 0)
		push_integer(1);
	else
		push_integer(0);
}

void
eval_not(void)
{
	push(cadr(p1));
	evalp();
	p1 = pop();
	if (iszero(p1))
		push_integer(1);
	else
		push_integer(0);
}

void
eval_and(void)
{
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalp();
		p2 = pop();
		if (iszero(p2)) {
			push_integer(0);
			return;
		}
		p1 = cdr(p1);
	}
	push_integer(1);
}

void
eval_or(void)
{
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalp();
		p2 = pop();
		if (!iszero(p2)) {
			push_integer(1);
			return;
		}
		p1 = cdr(p1);
	}
	push_integer(0);
}

// use subtract for cases like A < A + 1

int
cmp_args(void)
{
	int t;

	push(cadr(p1));
	eval();
	push(caddr(p1));
	eval();
	subtract();
	p1 = pop();

	// try floating point if necessary

	if (p1->k != RATIONAL && p1->k != DOUBLE) {
		push(p1);
		float_expr();
		p1 = pop();
	}

	if (iszero(p1))
		return 0;

	switch (p1->k) {
	case RATIONAL:
		if (p1->sign == MMINUS)
			t = -1;
		else
			t = 1;
		break;
	case DOUBLE:
		if (p1->u.d < 0.0)
			t = -1;
		else
			t = 1;
		break;
	default:
		stop("relational operator: cannot determine due to non-numerical comparison");
		t = 0;
	}

	return t;
}
