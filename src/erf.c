void
eval_erf(struct atom *p1)
{
	push(cadr(p1));
	eval();
	erffunc();
}

void
erffunc(void)
{
	double d;
	struct atom *p1;

	p1 = pop();

	if (isdouble(p1)) {
		d = erf(p1->u.d);
		push_double(d);
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	if (isnegativeterm(p1)) {
		push_symbol(ERF);
		push(p1);
		negate();
		list(2);
		negate();
		return;
	}

	push_symbol(ERF);
	push(p1);
	list(2);
}
