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
	push_integer(0);
}

void
eval_check(void)
{
	push(cadr(p1));
	evalp();
	p1 = pop();
	if (iszero(p1))
		stop("check");
	push_symbol(NIL); // no result is printed
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
			push_integer(1);
		else
			push_integer(0);
		return;
	}

	if (iszero(p2)) {
		push_integer(0);
		return;
	}

	if (istensor(p1) && istensor(p2) && !compatible_tensors(p1, p2)) {
		push_integer(0);
		return;
	}

	// try this first

	if (equal(p1, p2)) {
		push_integer(1);
		return;
	}

	// subtraction might simplify to zero

	push(p1);
	push(p2);
	subtract();

	p1 = pop();

	if (iszero(p1))
		push_integer(1);
	else
		push_integer(0);
}

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

int
cmp_args(void)
{
	int t;

	push(cadr(p1));
	eval();

	push(caddr(p1));
	eval();

	p2 = pop();
	p1 = pop();

	if (istensor(p1) || istensor(p2))
		stop("tensor comparison");

	push(p1);
	push(p2);
	subtract();
	p1 = pop();

	if (!isnum(p1)) {
		push(p1);
		sfloat(); // try converting pi and e
		p1 = pop();
		if (!isnum(p1))
			stop("non-numerical comparison");
	}

	if (iszero(p1))
		t = 0;
	else if (isrational(p1))
		t = (p1->sign == MMINUS) ? -1 : 1;
	else
		t = (p1->u.d < 0.0) ? -1 : 1;

	return t;
}

// like eval() except '=' is evaluated as '=='

void
evalp(void)
{
	save();
	p1 = pop();
	if (car(p1) == symbol(SETQ))
		eval_testeq();
	else {
		push(p1);
		eval();
	}
	restore();
}
