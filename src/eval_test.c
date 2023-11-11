void
eval_test(struct atom *p1)
{
	struct atom *p2;
	p1 = cdr(p1);
	while (iscons(p1)) {
		if (!iscons(cdr(p1))) {
			push(car(p1)); // default case
			evalf();
			return;
		}
		push(car(p1));
		evalp();
		p2 = pop();
		if (!iszero(p2)) {
			push(cadr(p1));
			evalf();
			return;
		}
		p1 = cddr(p1);
	}
	push_symbol(NIL);
}

void
eval_testeq(struct atom *p1)
{
	push(cadr(p1));
	evalg();
	push(caddr(p1));
	evalg();
	subtract();
	simplify();
	p1 = pop();
	if (iszero(p1))
		push_integer(1);
	else
		push_integer(0);
}

void
eval_testge(struct atom *p1)
{
	if (cmp_args(p1) >= 0)
		push_integer(1);
	else
		push_integer(0);
}

void
eval_testgt(struct atom *p1)
{
	if (cmp_args(p1) > 0)
		push_integer(1);
	else
		push_integer(0);
}

void
eval_testle(struct atom *p1)
{
	if (cmp_args(p1) <= 0)
		push_integer(1);
	else
		push_integer(0);
}

void
eval_testlt(struct atom *p1)
{
	if (cmp_args(p1) < 0)
		push_integer(1);
	else
		push_integer(0);
}

int
cmp_args(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	push(caddr(p1));
	evalf();
	subtract();
	simplify();
	floatfunc();
	p1 = pop();
	if (iszero(p1))
		return 0;
	if (!isnum(p1))
		stopf("compare err");
	if (isnegativenumber(p1))
		return -1;
	else
		return 1;
}
